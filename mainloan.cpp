#include "/Users/macbookpro/Documents/contracts/mainloan/mainloan.hpp"

void mainloan::addborrower(name acc_name, uint64_t b_id, string location,
                    uint64_t b_phone, uint64_t loan_individual,
                    uint64_t b_balance)
{
  print("Adding borrower: ", acc_name);
  require_auth( _self ); //authorization of the contract account

    eosio::check(loan_individual >= 0, "loan must be positive");
    eosio::check(b_balance >= 0, "balance must be positive");

    borr_table.emplace(get_self(), [&](auto& b) {
        b.acc_name = acc_name;
        b.b_id = b_id;
        b.location = location;
        b.b_phone = b_phone;
        b.loan_individual = loan_individual;
        b.credit_amnt = b_balance;
  });
}

void mainloan::adduwr(name acc_name, uint64_t acc_id, uint64_t balance)
{
    print("Adding underwriter", acc_name);
    require_auth( _self );

      eosio::check(balance >= 0, "balance must be positive");
      //eosio::check(value_score>=300 && value_score<=900, "credit score is always postive and ranges from 300-900");

      uwr_table.emplace(get_self(), [&](auto& u) {
          u.acc_name = acc_name;
          u.acc_id = acc_id;
          u.balance = balance;
      });
}

void mainloan::getborrower(name acc_name){

  auto borrower = borr_table.get(acc_name.value);
  eosio::check(borrower.acc_name==acc_name, "Borrower doesn't exist.");

  eosio::print("Borrower Details: ", borrower.acc_name);
  eosio::print("ID: ", borrower.b_id);
  eosio::print("Location: ", borrower.location);
  eosio::print("Phone Number: ", borrower.b_phone);
  eosio::print("Loan Individual: ", borrower.loan_individual);
  eosio::print("Balance: ", borrower.credit_amnt);
  eosio::print("Credit Score: ", borrower.credit_score);
}

void mainloan::addloan(name uwr_name, name borr_name, uint64_t loan_amnt, uint64_t rate, uint64_t pay_time){

  require_auth( _self );
  eosio::check(loan_amnt>0, "Cannot loan in negatives!");
  eosio::check(rate>=0, "Interst rate cannot be negative!");
  eosio::check(pay_time>0, "Cannot be zero or negative!");
  auto borrower = borr_table.find(borr_name.value);
  auto uwr = uwr_table.find(uwr_name.value);
  eosio::check(borrower!=borr_table.end(), "Borrower doesn't exist.");
  eosio::check(uwr!=uwr_table.end(), "Lender doesn't exist.");

  loan_table.emplace(get_self(), [&](auto &l){
    l.loan_id = loan_table.available_primary_key();
    l.uwr_name = uwr_name;
    l.uwr_id = uwr->acc_id;
    l.lending_amount = loan_amnt;
    l.borr_name = borr_name;
    l.borr_id = borrower->b_id;
    l.interest_rate = rate;
    l.payment_time = pay_time;
    l.emi = loan_amnt*rate*pow(1+rate, pay_time)/(pow(1+rate, pay_time)-1);
    l.return_value = l.emi*pay_time;
  });
  print("Loan Added");
}

void mainloan::getuwr(name acc_name){

  auto underwriter = uwr_table.get(acc_name.value);
  eosio::check(underwriter.acc_name==acc_name, "Underwriter doesn't exist.");

  eosio::print("underwriter Details: ", underwriter.acc_name);
  eosio::print(" ID: ", underwriter.acc_id);
  eosio::print("Balance: ", underwriter.balance);
}

void mainloan::defincr(name from, uint64_t loanpm, name to)
{
        require_auth(from);
        auto itr = borr_table.get(to.value);
        auto itr2 = uwr_table.find(from.value);
        eosio::check(itr.acc_name==to, "Borrower doesn't exist.");
        eosio::check(itr2!=uwr_table.end(), "Lender doesn't exist.");
        eosio::check(loanpm>0, "Cannot loan in negatives!");

        print("Deferred loan from ", from, " for credit of ", loanpm, " to ", to);
        itr.credit_amnt += loanpm;
}

void mainloan::send(name from, bool check, name to, uint64_t loanpm)
{
        require_auth(from);
        eosio::check(check==1, "Borrower has not paid last month's return amount.");

        eosio::transaction t{};
        t.actions.emplace_back(
            permission_level(from, "active"_n),
            _self,
            "defincr"_n,
            std::make_tuple(from, loanpm, to));

       t.delay_sec = 30*24*60*60;   //delay in seconds => 1 month in sec

       t.send(now(), from /*, false */);

       // deferred_table.emplace(get_self(), [&](auto &d){
       //   d.loan_id = deferred_table.available_primary_key();
       //   d.uwr_name = from;
       //   d.lending_amount = loanpm;
       //   d.borr_name = to;
       //   d.
       // });

        print("Scheduled with a delay of 30 days.");
}

void mainloan::onanerror(const onerror &error){

        print("Resending Transaction: ", error.sender_id);
        def_counter++;
        if (def_counter<=3){
            eosio::transaction dtrx = error.unpack_sent_trx();
            dtrx.delay_sec = 2;
            dtrx.send(now(), _self);
        }
        else{
            print("Txn couldnt take place");
        }
}

// extern "C" void apply(uint64_t receiver, uint64_t code, uint64_t action){
//   if (code=="eosio"_n.value && action=="onanerror"_n.value){
//     eosio::execute_action(eosio::name(receiver), eosio::name(code),
//       &mainloan::onanerror);
//   }
// }

///namespace eosio
EOSIO_DISPATCH(mainloan, (addborrower)(adduwr)(addloan)(getborrower)(getuwr)(defincr)(send))
