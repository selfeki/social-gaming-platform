State Interpreter:: operator()(GlobalMessage const& gM) const{
      return State(gM.message.getData(), PlayersListPacket<Player>(players));
}

State Interpreter:: operator()(Message<Player> const& m) const{
      return State(m.message.getData(), m.recipents );
}

State Interpreter:: operator()(Scores const& sc) const{ 
    	std::string info = "" ;
    	for(auto player : players){
    		if(sc.playerAttribute.getData() == "wins"){
    			info.append(player.name + " " + std::to_string(player.wins));
    		}
    	}
    	return State(DataPacket(info), PlayersListPacket<Player>(players));
  } 

  State Interpreter:: operator()( InputText<Player> const& iT) const{
    	return State(iT.dP.getData(), PlayersListPacket<Player>(iT.player), iT.playerAttribute) ;
    }

    State Interpreter:: takeInput(int timeout, PlayersListPacket<Player> player, std::string attribute){
    	int seconds = 0 ;
    	std::string str = "" ;
    	if(timeout){
    		while(seconds <= timeout){
	    		sleep(1);
	    		seconds++;
	    		std::getline(std::cin, str);
	    		if(str.size() > 0){
	    			break ;
	    		}
	    	}
    	}
    	else{
    		std::getline(std::cin, str);
    	}
    	return State(DataPacket(str), player);
    }