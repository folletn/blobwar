#include "strategy.h"

void Strategy::applyMove (const movement& mv) {
     if ((mv.ox-mv.nx+1)*(mv.ox-mv.nx-1)>0 || (mv.oy-mv.ny+1)*(mv.oy-mv.ny-1)>0) {
	  _blobs.set(mv.ox, mv.oy,-1);
     }
     for(int x = std::max(0,mv.nx-1) ; x <= std::min(7,mv.nx+1) ; x++) {
	  for(int y = std::max(0,mv.ny-1) ; y <= std::min(7,mv.ny+1) ; y++) {
	       if (_holes.get(x, y)) continue;
	       if (_blobs.get(x, y) == 1-(int)_current_player) {
		    _blobs.set(x, y, (int)_current_player);
	       }
	  }
     }
     _blobs.set(mv.nx, mv.ny,_current_player);
     _current_player = 1-(int)_current_player;
}

Sint32 Strategy::estimateCurrentScore (Uint16 player) const {
     Sint32 score = 0;
     //we prefer to capture a blob than to duplicate (aggresive bot).
     for(int x = 0 ; x < 8 ; x++) {
	  for(int y = 0 ;  y < 8 ; y++) {
	       if (_holes.get(x, y)) continue;
	       if (_blobs.get(x, y) == (int) player) {
		    score += 100;
	       } else if (_blobs.get(x, y) == 1 - (int) player) {
		    score -= 101;
	       }
	  }
     }
     return score;
}

vector<movement>& Strategy::computeValidMoves(vector<movement>& valid_moves) {
     //The following code find valid moves.
     movement mv(0,0,0,0);
     //iterate on starting position
     for(mv.ox = 0 ; mv.ox < 8 ; mv.ox++) {
	  for(mv.oy = 0 ; mv.oy < 8 ; mv.oy++) {
	       if (_blobs.get(mv.ox, mv.oy) == (int) _current_player) {
		    //iterate on possible destinations
		    for(mv.nx = std::max(0,mv.ox-2) ; mv.nx <= std::min(7,mv.ox+2) ; mv.nx++) {
			 for(mv.ny = std::max(0,mv.oy-2) ; mv.ny <= std::min(7,mv.oy+2) ; mv.ny++) {
			      if (_holes.get(mv.nx, mv.ny)) continue;
			      if (_blobs.get(mv.nx, mv.ny) == -1) {
				   valid_moves.push_back(mv);
			      }
			 }
		    }
	       }
	  }
     }
     return valid_moves;
}


Sint32 Strategy::max(int depth, Uint16 player, Sint32 alpha, Sint32 beta) {
     if (depth == 0) return estimateCurrentScore(player);

     Sint32 scoreMax;
     vector<movement> valid_moves;
     movement mv(0,0,0,0);
     valid_moves = computeValidMoves(valid_moves);
     if (valid_moves.size() == 0) return estimateCurrentScore(player);
     //The following code find the max score.
     Strategy strat1 = Strategy(_blobs, _holes, _current_player, _saveBestMove);
     strat1.applyMove(valid_moves[0]);
     Sint32 score;
     score  = strat1.min(depth - 1, player, alpha, beta);
     scoreMax=score;
     if(scoreMax>beta) return scoreMax;
     //Si le premier noeud du max n'est pas une coupure beta
     //lancement de n-1 thread executés en parallèle
     Sint32 table[valid_moves.size()]; 
     table[0]=score;
     for (unsigned int i = 1; i < valid_moves.size(); i++) {
	  Strategy strat = Strategy(_blobs, _holes, _current_player, _saveBestMove);
	  strat.applyMove(valid_moves[i]);
	  score = strat.min(depth - 1, player, scoreMax, beta);
	  table[i]=score;
     }
     for (unsigned int j = 1; j < valid_moves.size();j++){
     	  if(scoreMax < table[j]) scoreMax=table[j];
     }
  
     return scoreMax;
}

Sint32 Strategy::min(int depth, Uint16 player, Sint32 alpha, Sint32 beta) {
     if (depth == 0) return estimateCurrentScore(player);

     Sint32 scoreMin;
     vector<movement> valid_moves;
     movement mv(0,0,0,0);
     valid_moves = computeValidMoves(valid_moves);
     if (valid_moves.size() == 0) return estimateCurrentScore(player);
     //The following code find the max score.
     Strategy strat1 = Strategy(_blobs, _holes, _current_player, _saveBestMove);
     strat1.applyMove(valid_moves[0]);
     Sint32 score;
     score = strat1.max(depth - 1, player, alpha, beta);
     scoreMin=score;
     if(scoreMin<alpha) return scoreMin;
     //Si le premier noeud du min n'est pas une coupure alpha
     //lancement de n-1 thread executés en parallèle
     Sint32 table[valid_moves.size()];
     table[0]=score;
     for (unsigned int i = 1; i < valid_moves.size(); i++) {
	  Strategy strat = Strategy(_blobs, _holes, _current_player, _saveBestMove);
	  strat.applyMove(valid_moves[i]);
	  score = strat.max(depth - 1, player, alpha, scoreMin);
	  table[i]=score;
     }
     for (unsigned int j = 1; j < valid_moves.size();j++){
     	  if(scoreMin > table[j]) scoreMin=table[j];
     }
     return scoreMin;
}


// Sint32 Strategy::max(int depth, Uint16 player, Sint32 alpha, Sint32 beta) {
//   if (depth == 0) return estimateCurrentScore(player);
//   Sint32 scoreMAX = -65000;
//   vector<movement> valid_moves;
//   movement mv(0,0,0,0);
//   valid_moves = computeValidMoves(valid_moves);
//   if (valid_moves.size() == 0) return estimateCurrentScore(player);
//   //The following code find the max score.
//   Sint32 score;
//   Sint32 newalpha = alpha;
//   for (unsigned int i = 0; i < valid_moves.size(); i++) {
//     Strategy strat = Strategy(_blobs, _holes, _current_player, _saveBestMove);
//     strat.applyMove(valid_moves[i]);
//     score = strat.min(depth - 1, player, newalpha, beta);
//     if (score > scoreMAX) {
//       scoreMAX = score;
//       newalpha = scoreMAX;
//     }
//     //coupure beta
//     if (scoreMAX > beta) {
//       return scoreMAX;
//     }
//   }
//   return scoreMAX;
// }

// Sint32 Strategy::min(int depth, Uint16 player, Sint32 alpha, Sint32 beta) {
//   if (depth == 0) return estimateCurrentScore(player);
//   Sint32 scoreMIN = 65000;
//   vector<movement> valid_moves;
//   movement mv(0,0,0,0);
//   valid_moves = computeValidMoves(valid_moves);
//   if (valid_moves.size() == 0) return estimateCurrentScore(player);
//   //The following code find the min score.
//   Sint32 score;
//   Sint32 newbeta = beta;
//   for (unsigned int i = 0; i < valid_moves.size(); i++) {
//     Strategy strat = Strategy(_blobs, _holes, _current_player, _saveBestMove);
//     strat.applyMove(valid_moves[i]);
//     score = strat.max(depth - 1, player, alpha, newbeta);
//     if (score < scoreMIN) {
//       scoreMIN = score;
//       newbeta = scoreMIN;
//     }
//     //coupure alpha
//     if (scoreMIN < alpha) {
//       return scoreMIN;
//     }
//   }
//   return scoreMIN;
// }

movement Strategy::computeBestMove (int depth, Uint16 player) {
     Sint32 scoreMAX = -65000;
     vector<movement> valid_moves;
  
     //The following code find valid moves.
     movement mv(0,0,0,0);
     //iterate on starting position
     for(mv.ox = 0 ; mv.ox < 8 ; mv.ox++) {
	  for(mv.oy = 0 ; mv.oy < 8 ; mv.oy++) {
	       if (_blobs.get(mv.ox, mv.oy) == (int) _current_player) {
		    //iterate on possible destinations
		    for(mv.nx = std::max(0,mv.ox-2) ; mv.nx <= std::min(7,mv.ox+2) ; mv.nx++) {
			 for(mv.ny = std::max(0,mv.oy-2) ; mv.ny <= std::min(7,mv.oy+2) ; mv.ny++) {
			      if (_holes.get(mv.nx, mv.ny)) continue;
			      if (_blobs.get(mv.nx, mv.ny) == -1) {
				   valid_moves.push_back(mv);
			      }
			 }
		    }
	       }
	  }
     }

     //The following code find the move with the best score calculated by MIN/MAX
     Sint32 score;
     Sint32 alpha = -66000;
     Sint32 beta = 66000;
     for (unsigned int i = 0; i < valid_moves.size(); i++) {
	  Strategy strat = Strategy(_blobs, _holes, _current_player, _saveBestMove);
	  strat.applyMove(valid_moves[i]);
	  score = strat.min(depth - 1, player, alpha, beta);
	  if (score > scoreMAX) {
	       scoreMAX = score;
	       mv = valid_moves[i];
	       alpha = scoreMAX;
	  }
     }
     return mv;
}

void Strategy::computeBestMove () {
     //We can change depth here
     movement mv(computeBestMove(4, _current_player));
     _saveBestMove(mv);
     return;
}


