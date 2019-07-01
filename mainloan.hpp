#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <string>

using namespace eosio;
using namespace std;

CONTRACT main_loan : public contract{
  private:
    //table definitions

    TABLE borrower_info{
      name acc_name;
      uint64_t b_id; //hash
      vector <string> location; //geohash or coordinates
      uint64_t b_phone;
      uint64_t group_type;
      asset loan_individual;
      asset b_balance; //borrower account balance
      uint64_t group_id; //hash function
      uint64_t credit_score;

      auto primary_key()const {
        return acc_name.value;
      }
      uint64_t by_group_id() const{
        return group_id;
      }
    };

    TABLE group_info{ //group leader should init this table ;;;;; primary key=group id
      uint64_t group_id;
      asset total_loan; //total loan of group
      vector <string> member_names; //name is ended with '/' //hash of this is the group_id //CHANGE: make array //DONE

      uint64_t primary_key() const{
        return group_id;
      }
    };

    TABLE shg_savings{ //NOT CONSIDERING
      uint64_t group_id;
      uint64_t saving_amnt;

      uint64_t primary_key() const{
        return group_id;
      }
    };

    TABLE underwriter_info{
      name acc_name;
      uint64_t acc_id;
      asset balance;
      uint64_t value_score;


      auto primary_key() const{
        return acc_name;
      }
    };

    TABLE lender_info{
      name acc_name;
      uint64_t acc_id;
      asset balance;

      auto primary_key() const{
        return acc_name;
      }
    };

    TABLE relayer_info{
      name acc_name;
      uint64_t acc_id;
      asset balance;

      auto primary_key() const{
        return acc_name;
      }
    };

    TABLE loan_info{ //loan id needed to put
      name acc_name;
      asset lending_amount;
      uint64_t lent_group_id;
      uint64_t interest_rate;
      uint64_t payment_time;
      uint64_t status=0; //0-incomplete 1-complete

      auto primary_key() const{
        return acc_name;
      }
    };

    //typedefs
    typedef eosio::multi_index<"borrower"_n, borrower_info,
                                eosio::indexed_by<"groupId"_n,
                                                  const_mem_fun<borrower_info, uint64_t, &borrower_info::by_group_id>>> borrower; //check
    typedef eosio::multi_index<"group"_n, group_info> group;
    typedef eosio::multi_index<"shg"_n, shg_savings> shg; //NOT CONSIDERING
    typedef eosio::multi_index<"underwriter"_n, underwriter_info> underwriter;
    typedef eosio::multi_index<"relayer"_n, relayer_info> relayer;
    typedef eosio::multi_index<"lender"_n, lender_info> lender;
    typedef eosio::multi_index<"loan"_n, loan_info> loan;

    //init
    borrower borr_table;
    group group_table;
    shg shg_table;
    underwriter uwr_table;
    relayer relayer_table;
    lender lender_table;
    loan loan_table;


    vector <string> location; //geohash or coordinates
    uint64_t b_phone;
    uint64_t group_type;
    asset loan_individual;
    asset b_balance; //borrower account balance
    uint64_t group_id; //hash function
    uint64_t credit_score;



  public:
    using contract::contract;

    ACTION add_borrower(name acc_name, uint64_t b_id, vector<string> location,
                        uint64_t b_phone, uint64_t group_type, asset loan_individual,
                        asset b_balance, uint64_t group_id, uint64_t credit_score);


};
