#!/usr/bin/env bash
set -e -o pipefail

# ----------------------------------------------------------------------------------------------------------------------
# Functions:
# ----------------------------------------------------------------------------------------------------------------------

timestamp() {
python3 - "$1" <<EOL
from datetime import datetime;
import sys;

dt_format = "%Y-%m-%d"
dt_object = datetime.strptime(sys.argv[1], dt_format)
print(int(datetime.timestamp(dt_object)))
EOL
}

printstatus() {
    printf "\x1B[G\x1B[2K\x1B[33m%s \x1B[0m%s\x1B[0m" "$1" "$2" 1>&2
}


# ----------------------------------------------------------------------------------------------------------------------
# Args:
# ----------------------------------------------------------------------------------------------------------------------

if [[ "$#" -ne 3 ]] || ! timestamp "$3" &>/dev/null; then
  printf "\x1B[31mInvalid arguments provided.\x1B[0m\n"
  printf "\x1B[31mUsage: \x1B[0m%s" "$0"
  printf " [\x1B[4m%s\x1B[24m%s\x1B[37m%s\x1B[0m]" \
    "dir"    ""   "" \
    "author" ": " "you@example.com" \
    "since"  ": " "YYYY-MM-DD"
  printf "\n"
  printf "\x1B[31mEnv:\x1B[0m\n"
  printf "    \x1B[4m%s\x1B[24m\x1B[37m%s\x1B[0m  \x1B[0m-- %s\x1B[0m\n" \
    "CLOC_TOOL" "       sloccount" "The line counting tool to use." \
    "SKIP_DIRECTIVES" " true     " "Whether to skip preprocessor directives." \
    "SKIP_COMMENTS" "   true     " "Whether to skip single-line comments." \
    "SKIP_TRIVIAL" "    true     " "Whether to skip trivial lines." \
    "SKIP_EMPTY" "      true     " "Whether to skip empty lines."
  exit 1
fi

TARGET_DIR="$1"
TARGET_AUTHOR="$2"
TARGET_TIMESTAMP="$(timestamp "$3")"

if [[ -z "$CLOC_TOOL" ]]; then
  CLOC_TOOL=sloccount
fi

if [[ -z "$SKIP_DIRECTIVES" ]]; then SKIP_DIRECTIVES=true; fi
if [[ -z "$SKIP_COMMENTS" ]];   then SKIP_COMMENTS=true; fi
if [[ -z "$SKIP_TRIVIAL" ]];    then SKIP_TRIVIAL=true; fi
if [[ -z "$SKIP_EMPTY" ]];      then SKIP_EMPTY=true; fi

# ----------------------------------------------------------------------------------------------------------------------
# Main:
# ----------------------------------------------------------------------------------------------------------------------

# Create a temporary file for all the user's lines.
TEMPFILE="$(mktemp)"
LINEFILE="${TEMPFILE}.cpp"
mv "$TEMPFILE" "$LINEFILE"

# Git blame and output the user's unique lines to the temp file.
({
  printf "\x1B[34mScanning \x1B[35m%s\x1B[34m for commits by \x1B[35m%s\x1B[34m.\n" "$TARGET_DIR" "$TARGET_AUTHOR" 1>&2

  while read -r file; do
    printstatus "$file"

    CURRENT_AUTHOR=''
    CURRENT_TIMESTAMP=0

    while IFS='' read -r line; do
      # Parse the `author-mail` line.
      if [[ "$line" =~ ^author-mail ]]; then
        CURRENT_AUTHOR="$(sed 's/author-mail <\(.*\)>$/\1/' <<< "$line")"
        continue
      fi

      # Parse the `author-time` line.
      if [[ "$line" =~ ^author-time ]]; then
        CURRENT_TIMESTAMP="$(sed 's/author-time \(.*\)$/\1/' <<< "$line")"
        continue
      fi

      # If the commit author is not the target author, we don't count the line.
      # If the commit timestamp is less than the target timestamp, we don't count the line.
      if [[ "$CURRENT_AUTHOR" != "$TARGET_AUTHOR" || "$CURRENT_TIMESTAMP" -lt "$TARGET_TIMESTAMP" ]]; then
        continue
      fi

      # If the line is a content line, we print it.
      if [[ "$line" =~ ^[[:space:]] ]]; then
        echo "$line"
      fi
    done < <(git blame "$file" --porcelain)
  done < <(find "${TARGET_DIR}" -type f \
    \( -iname '*.cpp' -o -iname '*.hpp' -o -iname '*.cc' -o  -iname '*.h' \) \
    -not \( -ipath '*/node_modules/*' \)
  )

  # Print a final newline.
  printf "\x1B[G\x1B[2K\x1B[34m%s\x1B[0m\n" "Done." 1>&2
}) > "$LINEFILE"

# Filter out lines based on our options.
{
  SED_PATTERNS=()

  # Skip preprocessor directives.
  if [[ "$SKIP_DIRECTIVES" = true ]]; then
    SED_PATTERNS+=('/^[[:space:]]*#.*$/d')
  fi

  # Skip single-line comments.
  if [[ "$SKIP_EMPTY" = true ]]; then
    SED_PATTERNS+=('/^[[:space:]]*\/\/.*$/d')
  fi

  # Skip empty lines.
  if [[ "$SKIP_EMPTY" = true ]]; then
    SED_PATTERNS+=('/^[[:space:]]*$/d')
  fi

  # Skip trivial lines.
  # - `{`
  # - `}`
  # - `using ...`
  # - `namespace ...`
  # - `public:`
  # - `private:`
  # - `protected:`
  # - `struct ...;`
  # - `class ...;`
  if [[ "$SKIP_EMPTY" = true ]]; then
    SED_PATTERNS+=('/^[[:space:]]*{[[:space:]]*$/d')
    SED_PATTERNS+=('/^[[:space:]]*}[[:space:]]*$/d')
    SED_PATTERNS+=('/^[[:space:]]*using[[:space:]].*/d')
    SED_PATTERNS+=('/^[[:space:]]*namespace[[:space:]].*{/d')
    SED_PATTERNS+=('/^[[:space:]]*private:[[:space:]]*$/d')
    SED_PATTERNS+=('/^[[:space:]]*(public|private|protected):[[:space:]]*$/d')
    SED_PATTERNS+=('/^[[:space:]]*(struct|class|union|enum|enum class).*\;.*$/d')
  fi

  # Perform filter.
  sed -i "bak" "$(printf '%s; ' "${SED_PATTERNS[@]}")" "$LINEFILE"
}



# Read the linefile.
"$CLOC_TOOL" "$LINEFILE"

# Remove the tempfile and exit.
rm "$LINEFILE"
