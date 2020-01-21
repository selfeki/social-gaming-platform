#!/usr/bin/env bash
# ----------------------------------------------------------------------------------------------------------------------
# <project name>, copyright 2020.
# A script to determine project information.
# ----------------------------------------------------------------------------------------------------------------------
set -e -o pipefail
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../" && pwd)"
PROJECT_TARGET_DIR="${PROJECT_DIR}/target/{{system}}/{{type}}"
PROJECT_NAME="$({ grep -i "project\s*(.*)" < "${PROJECT_DIR}/CMakeLists.txt" | sed 's/^.*(\(.*\)).*$/\1/; s/['\"\'']//'; } || true)"

printf "%s=%q\n" \
	PROJECT_NAME       "$PROJECT_NAME" \
	PROJECT_DIR        "$PROJECT_DIR" \
	PROJECT_TARGET_DIR "$PROJECT_TARGET_DIR"

