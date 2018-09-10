#pragma once
#include <eosio/chain/block_state.hpp>
#include <boost/signals2/signal.hpp>

namespace eosio { namespace chain {

   using boost::signals2::signal;

   struct fork_database_impl;

   typedef vector<block_state_ptr> branch_type;

   /**
    * @class fork_database
    * @brief manages light-weight state for all potential unconfirmed forks
    *
    * As new blocks are received, they are pushed into the fork database. The fork
    * database tracks the longest chain and the last irreversible block number. All
    * blocks older than the last irreversible block are freed after emitting the
    * irreversible signal.
    */
   class fork_database {
      public:

         fork_database( const fc::path& data_dir );
         ~fork_database();

         void close();

         block_state_ptr  get_block(const block_id_type& id)const;

         /**
          *  Purges any existing blocks from the fork database and resets the root block_header_state to the provided value.
          *  The head will also be reset to point to the root.
          */
         void            reset( const block_header_state& root_bhs );

         /**
          *  Advance root block forward to some other block in the tree.
          */
         void            advance_root( const block_id_type& id );

         /**
          *  Add block state to fork database.
          *  Must link to existing block in fork database or the root.
          */
         void            add( block_state_ptr next_block );

         void            add( const header_confirmation& c );

         void            remove( const block_id_type& id );

         const block_state_ptr& root()const;
         const block_state_ptr& head()const;

         /**
          *  Returns the sequence of block states in the branch from the root block (exclusive) to the block with an id of `h` (inclusive) in the reversed order.
          *  A block with an id of `h` must exist in the fork database otherwise this method will throw an exception.
          */
         branch_type     fetch_branch( const block_id_type& h )const;

         /**
          *  Given two head blocks, return two branches of the fork graph that
          *  end with a common ancestor (same prior block)
          */
         pair< branch_type, branch_type >  fetch_branch_from( const block_id_type& first,
                                                              const block_id_type& second )const;


         /**
          * If the block is invalid, it will be removed. If it is valid, then blocks older
          * than the LIB are pruned after emitting irreversible signal.
          */
         void set_validity( const block_state_ptr& h, bool valid );

      private:
         void set_bft_irreversible( block_id_type id );
         unique_ptr<fork_database_impl> my;
   };

} } /// eosio::chain
