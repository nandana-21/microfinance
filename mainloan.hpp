#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>

using namespace eosio;
using namespace std;

CONTRACT loan : public contract{
  private:
    //table definitions

    TABLE borrower_info{
      name acc_name;
      uint64_t b_id; //hash
      datastream <const char *> location;
      uint64_t b_phone;
      uint64_t group_type;
      asset loan_required;
      uint64_t group_id; //hash function
      uint64_t credit_score;

      auto primary_key()const {
        return acc_name.value;
      }
    }



    TABLE group_info{ //group leader should init this table ;;;;; primary key=group id
      uint64_t group_id;
      asset loan_required; //total loan of group
      name acc_name; //group leader name
      datastream<const char*> member_names; //hash of this is the group_id

      uint64_t primary_key() const{
        return group_id;
      }
    }

    TABLE shg_savings{
      uint64_t group_id;
      uint64_t saving_amnt;

      uint64_t primary_key() const{
        return group_id;
      }
    }

    TABLE underwriter_info{
      name acc_name;
      uint64_t acc_id;
      asset balance;
      uint64_t value_score;


      auto primary_key() const{
        return acc_name;
      }
    }

    TABLE lender_info{
      name acc_name;
      uint64_t acc_id;
      asset balance;

      auto primary_key() const{
        return acc_name;
      }
    }

    TABLE relayer{
      name acc_name;
      uint64_t acc_id;
      asset balance;

      auto primary_key() const{
        return acc_name;
      }
    }

    TABLE lender_lending_info{ //loan id needed to put
      name acc_name;
      asset lending_amount;
      name borr_name;
      uint64_t interest_rate;
      uint64_t payment_time;

      auto primary_key() const{
        return acc_name;
      }
    }

  public:

    ACTION member_to_leader()


}
