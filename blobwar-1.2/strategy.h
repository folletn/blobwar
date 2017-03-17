#ifndef __STRATEGY_H
#define __STRATEGY_H

#include "common.h"
#include "bidiarray.h"
#include "movement.h"



class Strategy {

private:
    //! array containing all blobs on the board
    bidiarray<Sint16> _blobs;
    //! an array of booleans indicating for each cell whether it is a hole or not.
    
    const bidiarray<bool>& _holes;
    //! Current player
    Uint16 _current_player;
    
    //! Call this function to save your best move.
    //! Multiple call can be done each turn,
    //! Only the last move saved will be used.
    void (*_saveBestMove)(movement&);

public:
        // Constructor from a current situation
    Strategy (bidiarray<Sint16>& blobs, 
              const bidiarray<bool>& holes,
              const Uint16 current_player,
              void (*saveBestMove)(movement&))
            : _blobs(blobs),_holes(holes), _current_player(current_player), _saveBestMove(saveBestMove)
        {
        }
    
              
    
        // Copy constructor
    Strategy (const Strategy& St)
            : _blobs(St._blobs), _holes(St._holes),_current_player(St._current_player) 
        {}
    
        // Destructor
    ~Strategy() {}
    
        /** 
         * Apply a move to the current state of blobs
         * Assumes that the move is valid
         */
    void applyMove (const movement& mv);

        /**
         * Compute the vector containing every possible moves
         */
    vector<movement>& computeValidMoves (vector<movement>& valid_moves) const;

        /**
         * Estimate the score of the current state of the game
         */
    Sint32 estimateCurrentScore (Uint16 player) const;

        /**
         * Compute the valid moves
         */
    vector<movement>& computeValidMoves(vector<movement>& valid_moves);
    
        /**
         * Max of Min/Max algo, then call min with depth-1.
	 * stop if depth == 0
         */
    Sint32 max(int depth, Uint16 player, Sint32 alpha, Sint32 beta);

        /**
         * Min of Min/Max algo, then call max with depth-1.
	 * stop if depth == 0
         */
    Sint32 min(int depth, Uint16 player, Sint32 alpha, Sint32 beta);
    
        /**
         * Find the valid moves, and then return the one with
	 * the max score according to minmax
         */
    movement computeBestMove (int depth, Uint16 player);
    
        /**
         * Find the best move.
         */
    void computeBestMove ();
    
    
};
#endif
