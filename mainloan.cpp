#include "/Users/macbookpro/Documents/contracts/mainloan/mainloan.hpp"

void mainloan::addborrower(name acc_name, uint64_t b_id, string location,
                    uint64_t b_phone, double loan_individual,
                    double b_balance)
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

void mainloan::adduwr(name acc_name, uint64_t acc_id, double balance)
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

void mainloan::notify(name user, string msg){
  require_auth(get_self());
  require_recipient(user);
}

void mainloan::addloan(uint64_t l_id, name uwr_name, name borr_name, double loan_amnt, double rate, uint64_t time_stmp){

  require_auth( _self );
  eosio::check(loan_amnt>0, "Cannot loan in negatives!");
  eosio::check(rate>=0, "Interst rate cannot be negative!");
  //eosio::check(pay_time>0, "Cannot be zero or negative!");
  auto borrower = borr_table.find(borr_name.value);
  auto uwr = uwr_table.find(uwr_name.value);
  eosio::check(borrower!=borr_table.end(), "Borrower doesn't exist.");
  eosio::check(uwr!=uwr_table.end(), "Lender doesn't exist.");
  eosio::check((uwr->balance)>loan_amnt, "Insufficient funds to lend.");
  print("hi");
  uint64_t id;
  loan_table.emplace(get_self(), [&](auto &l){
    l.loan_id = l_id;//loan_table.available_primary_key();
    id = l.loan_id;
    l.uwr_name = uwr_name;
    l.uwr_id = uwr->acc_id;
    l.lending_amount = loan_amnt;
    l.borr_name = borr_name;
    l.borr_id = borrower->b_id;
    l.interest_rate = rate;
    //l.payment_time = 30; //days
    l.inc_loan = (l.lending_amount)/4.0;
    //rate /= 1200;
    //l.emi = loan_amnt*rate*pow(1+rate, pay_time)/(pow(1+rate, pay_time)-1);
    //l.return_value = l.emi*pay_time;
    l.time_stmp = time_stmp;
    l.msg = to_string(now());
    //l.status = 0; //not completed
    //l.type = type;
  });
  sendinstl(id);
  checkperiod(id, 1);

  //add first disbursment in borrower account
  print("Loan Added");
}

void mainloan::addinstl(uint64_t loan_id, uint64_t paid_time, double instl_paid){
  require_auth(_self);
  //bool late = 0;
  bool check=0;     // to check if the first installment of the loan in repaid.
  double calc_days;
  bool minamnt;
  uint64_t disbursal_time;
  uint64_t excess;

  auto itr = loan_table.find(loan_id);
  auto itr2 = schedule_table.find((loan_id+1)*100+itr->instl_num-1);
  print("loanidtofind ", (loan_id+1)*100+itr->instl_num-1, "starttime: ", itr2->start_time);
  // auto id_recs = schedule_table.get_index<("byloanid"_n)>();
  // auto instl_recs = schedule_table.get_index<("byinstlnum"_n)>();
  // auto id_itr = id_recs.find(loan_id);
  // while()
  (itr2!=schedule_table.end()? disbursal_time=itr2->paid_time : disbursal_time=itr->time_stmp);
  print("disbursal tiemd: ", disbursal_time);
  calc_days = (paid_time-disbursal_time)/(1000*60*60*24.0);
  if (itr2!=schedule_table.end() && itr2->fee_next_instl!=0){
    ((instl_paid>((itr->interest_rate)/100*(int(calc_days)/365))+(itr2->fee_next_instl))? minamnt=1 : minamnt=0);
  }
  else{
    ((instl_paid>((itr->interest_rate)/100*(int(calc_days)/365)))? minamnt=1 : minamnt=0);
  }
  eosio::check(minamnt==1, "Minimum amount of the interest rate should be paid, please check if an earlier late fee has been charged.");
  if (itr2!=schedule_table.end()){
    eosio::check((itr2->installment_num)<=4, "Term for this loan has expired.");
  }

  //(paid_time>(disbursal_time+7*24*60*60*1000)? late=1 : late=0);
  eosio::check(itr!=loan_table.end(), "Loan doesn't exist.");

  // if (late != 1){
  //   calc_days = (paid_time-disbursal_time)/(1000*60*60*24);
  // }
  // else{
  //   calc_days = ((itr->payment_time)*30/(itr->terms));
  // }


  schedule_table.emplace(get_self(), [&](auto &s){
    (itr2!=schedule_table.end()? s.sch_id=itr2->sch_id+1 : s.sch_id=(loan_id+1)*100);          //schedule_table.available_primary_key();
    s.loan_id = loan_id;
    if (itr2!=schedule_table.end()){
      s.installment_num = (itr2->installment_num)+1;
    }
    else{
      s.installment_num++;
    }
    loan_table.modify(itr, _self, [&](auto& l){
      l.instl_num++;
    });
    uint64_t inum = s.installment_num;
    // (s.installment_num==4? check=1 : check=0); //to check if first installment is paid; installment==4 wont work then
    (s.installment_num==1? s.start_time=disbursal_time : s.start_time=itr2->paid_time);     //needs to change every week. //HARDCODE!!
    s.lent_amnt = itr->lending_amount;
    s.annual_interest = itr->interest_rate;
    s.total_time = itr->payment_time;
    s.instl_paid = instl_paid;
    s.paid_time = paid_time;
    s.ipd = (s.annual_interest)/(100*365);
    s.days = calc_days;
    (s.days>int((itr->payment_time)*30/(itr->terms))?
        s.interest_amnt=(s.ipd)*(int((itr->payment_time)*30/(itr->terms))) : s.interest_amnt=(s.ipd)*(int(s.days)));
    print("hi again: ", s.ipd*int(s.days));
    (s.installment_num==1? s.interest_amnt*=s.lent_amnt : s.interest_amnt*=itr2->remaining_amnt);
    s.principal_amnt = (s.instl_paid)-(s.interest_amnt);
    (s.installment_num==1? s.remaining_amnt=(s.lent_amnt)-(s.principal_amnt) : s.remaining_amnt= itr2->remaining_amnt-s.principal_amnt);
    if (itr->late_pay_fee!=0){
      s.fee_next_instl = itr->late_pay_fee;
      loan_table.modify(itr, _self, [&](auto& l){
        l.late_pay_fee=0;
        l.msg="modified";
      });
    }

    if (itr2!=schedule_table.end() && itr2->fee_next_instl!=0){
      s.remaining_amnt += itr2->fee_next_instl;
    }
    if (s.remaining_amnt<=0){
      check=1;
      excess = s.remaining_amnt;
    }
  });

  // sendinstl(loan_id);

  if (check==1){
    loan_table.modify(itr, _self, [&](auto &l){
      l.status = 1;
    });
    auto itr1 = borr_table.find(itr->borr_name.value);
    borr_table.modify(itr1, _self, [&](auto& b){
      b.credit_amnt-=excess;
    });
    sendinstl(loan_id);
  }

  // if (calc_days<(disbursal_time/(24*60*60*1000)+((itr->payment_time)*30/(itr->terms)))){
  //   checkperiod(loan_id, itr->instl_num+1);
  // }
  // else{
  //   checkperiod(loan_id, )
  // }

  checkperiod(loan_id, itr->instl_num+1);

  print("Installment added.");
}

void mainloan::sendinstl(uint64_t loan_id){
  require_auth(get_self());
  auto itr = loan_table.get(loan_id);

  eosio::transaction t{};
  eosio::print("  empty txn created.    ");
  t.actions.emplace_back(
      permission_level(get_self(), "active"_n),
      get_self(),
      "defincr"_n,
      std::make_tuple(itr.uwr_name, itr.lending_amount, itr.borr_name, loan_id));
  eosio::print("  action inserted in txn w delay set.   ");

 t.delay_sec = 10;//30*24*60*60;   //delay in seconds => 1 month in sec
 eosio::print(" delay set.    ");
 t.send(now(), get_self() /*, false */);
 eosio::print(" tnx sent.   ");
}

void mainloan::defincr(name from, double loanpm, name to, uint64_t loan_id)
{
        //eosio::print("Hello123");
        require_auth(get_self());

        auto itr = borr_table.find(to.value);
        auto itr1 = loan_table.find(loan_id);
        if (itr1->status==1 || itr1->disb_num==0){
          borr_table.modify(itr, _self, [&](auto& o){    //record, payer, new changed record
            o.credit_amnt -= loanpm;    //money disbursed
        //   //  return;
          });
          loan_table.modify(itr1, _self, [&](auto& l){
            l.disb_num += 1;
          });
        }
        // auto itr1 = loan_table.get(loan_id);
        // loan_table.modify(itr1, to, [&](auto &l){
        //   l.status=0;
        // });
        //}
        //checkstat(loan_id);
        //auto itr2 = uwr_table.find(from.value);

        // auto itr3 = loan_table.find(loan_id);
        // addloan(from, to, itr3->lending_amount, itr3->interest_rate, itr3->payment_time);
        // loan_table.modify(itr3, from, [&](auto& o){
        //   o.loan_instl += 1;
        // });
        eosio::print("New installment sent from ", from, " for credit of ", loanpm, " to ", to);

}

void mainloan::checkperiod(uint64_t loan_id, uint64_t instl_check, uint64_t delay){
  require_auth(get_self());
  auto itr = loan_table.get(loan_id);
  auto itr1 = loan_table.find(loan_id);

  eosio::transaction t{};
  eosio::print("  second txn created.    ");
  t.actions.emplace_back(
      permission_level(get_self(), "active"_n),
      get_self(),
      "checkpayment"_n,
      std::make_tuple(loan_id, instl_check)); //NEED TO CORRECT THIS
  eosio::print("  |||||action inserted in txn w delay set.   ");
  // if (delay==60*3){
  //   loan_table.modify(itr1, _self, [&](auto& l){
  //     l.msg.append("  3 min"+to_string(now()));
  //   });
  // }
  // else if (delay==60*5){
  //   loan_table.modify(itr1, _self, [&](auto& l){
  //     l.msg.append("  5 min"+to_string(now()));
  //   });
  // }
  // else{
  //   loan_table.modify(itr1, _self, [&](auto& l){
  //     l.msg.append("  pata nahi"+to_string(now()));
  //   });
  // }
 t.delay_sec = delay; //7 days=>1 installment  //30*24*60*60;   //delay in seconds => 1 month in sec
 eosio::print(" ||delay set.    ");
 t.send(now()+delay, get_self() /*, false */);
 eosio::print(" ||tnx sent.   ");

}

void mainloan::checkpayment(uint64_t loan_id, uint64_t instl_check){
  require_auth(_self);

  //auto index_w_loanid = schedule_table.get_index<"byloanid"_n>();//schedule_table.find(loan_id);
  auto itr = loan_table.find(loan_id);
  auto itr1 = schedule_table.find((loan_id+1)*100+instl_check-1);
  uint64_t fee = 0;
  if (itr1 == schedule_table.end()){     //check every day if it's paid //after it's paid, get total fine amount and add to next installment
    checkperiod(loan_id, instl_check, 60*2);//60*60*24);
    loan_table.modify(itr, _self, [&](auto& l){
      l.late_pay_fee += (l.interest_rate)/(100*365);
      l.msg.append(" {"+to_string(l.late_pay_fee)+"} "+to_string(now())+"||");
    });
    checkperiod(loan_id, instl_check+1);      //check next week's installment
    print("Late payment fees charged.");
  }
  else{   //first installment paid
    checkperiod(loan_id, instl_check+1);
  }
}

void mainloan::clearall(){
  require_auth(_self);
  // auto itr = borr_table.begin();
  // while (itr != borr_table.end()){
  //   itr = borr_table.erase(itr);
  // }
  // auto itr1 = uwr_table.begin();
  // while (itr1 != uwr_table.end()){
  //   itr1 = uwr_table.erase(itr1);
  // }
  auto itr2 = loan_table.begin();
  while (itr2 != loan_table.end()){
    itr2 = loan_table.erase(itr2);
  }
  auto itr3 = schedule_table.begin();
  while (itr3 != schedule_table.end()){
    itr3 = schedule_table.erase(itr3);
  }
  // auto itr = schedule_table.find(0);
  // schedule_table.modify(itr, _self, [&](auto& s){
    // l.lending_amount = 1200;
    // l.interest_rate = 10;
    // l.payment_time = 30; //days
    // l.inc_loan = (l.lending_amount)/4.0;
    //s.paid_time = 1562747553653;
    // s.days = (1562747553653-1562592888435)/(1000*60*60*24);
    // s.interest_amnt = (s.ipd)*(int(s.days))*(s.remaining_amnt);
    //l.status = false;
  // });
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

  //send("bubblebee123"_n, 1, acc_name, 1000, 1);
}

void mainloan::getuwr(name acc_name){

  auto underwriter = uwr_table.get(acc_name.value);
  eosio::check(underwriter.acc_name==acc_name, "Underwriter doesn't exist.");

  eosio::print("underwriter Details: ", underwriter.acc_name);
  eosio::print(" ID: ", underwriter.acc_id);
  eosio::print("Balance: ", underwriter.balance);
}

void mainloan::getloan(uint64_t loan_id){  //Loan ID to be taken or another key to be created!!

  auto loaninfo = loan_table.get(loan_id);
  eosio::check(loaninfo.loan_id==loan_id, "Loan ID doesn't exist.");

  eosio::print("loan details :", loaninfo.loan_id);
  eosio::print("        borrower name :", loaninfo.borr_name);
  eosio::print("        underwriter name :", loaninfo.uwr_name);
  eosio::print("        lending amount :", loaninfo.lending_amount);
  eosio::print("        Interst rate :", loaninfo.interest_rate);
  eosio::print("        Payment time :", loaninfo.payment_time);
  eosio::print("        loan increment :", loaninfo.inc_loan);
  eosio::print("        time stamp :", loaninfo.time_stmp);
  eosio::print("        Payment status :", loaninfo.status);
  //eosio::print("emi :", loaninfo.emi);
  //eosio::print("Time stamp: ", std::to_string(loaninfo.time_stmp));
  //eosio::print("Total amount to be returned", loaninfo.return_value);
}

void mainloan::getschedule(uint64_t sch_id){  //Loan ID to be taken or another key to be created!!

  auto schdinfo = schedule_table.get(sch_id);
  eosio::check(schdinfo.sch_id==sch_id, "Loan ID doesn't exist.");

  eosio::print("schedule details :", schdinfo.sch_id);
  eosio::print("        loan id :", schdinfo.loan_id);
  eosio::print("        installment number :", schdinfo.installment_num);
  eosio::print("        total lent amount :", schdinfo.lent_amnt);
  eosio::print("        disbursal time :", schdinfo.start_time);
  eosio::print("        annual interst  :", schdinfo.annual_interest);
  eosio::print("        total time :", schdinfo.total_time);
  eosio::print("        paid time :", schdinfo.paid_time);
  eosio::print("        installment paid :", schdinfo.instl_paid);
  eosio::print("        dsys :", schdinfo.days);
  eosio::print("        remaining amount :", schdinfo.remaining_amnt);
  eosio::print("        ipd :", schdinfo.ipd);
  eosio::print("        interest amnt :", schdinfo.interest_amnt);
  eosio::print("        principal amnt :", schdinfo.principal_amnt);
  //eosio::print("emi :", loaninfo.emi);
  //eosio::print("Time stamp: ", std::to_string(loaninfo.time_stmp));
  //eosio::print("Total amount to be returned", loaninfo.return_value);
}

//
// void mainloan::send(name from, bool check_stat, name to, double loanpm, uint64_t loan_id){
//         require_auth(from);
//         eosio::check(check_stat==1, "Borrower has not paid last month's return amount.");
//
//         eosio::transaction t{};
//         eosio::print("  empty txn created.    ");
//         t.actions.emplace_back(
//             permission_level(from, "active"_n),
//             _self,
//             "defincr"_n,
//             std::make_tuple(from, loanpm, to, loan_id));
//         eosio::print("  action inserted in txn w delay set.   ");
//
//        t.delay_sec = 5;//30*24*60*60;   //delay in seconds => 1 month in sec
//        eosio::print(" delay set.    ");
//        t.send(_self.value, from /*, false */);
//        eosio::print(" tnx sent.   ");
//        // deferred_table.emplace(get_self(), [&](auto &d){
//        //   d.loan_id = deferred_table.available_primary_key();
//        //   d.uwr_name = from;
//        //   d.lending_amount = loanpm;
//        //   d.borr_name = to;
//        //   d.
//        // });
//
//         eosio::print("Scheduled with a delay of 30 days.");
// }
//
void mainloan::onanerror(const onerror &error){

        print("Resending Transaction: ", error.sender_id);
        def_counter++;
        if (def_counter<=3){
            eosio::transaction dtrx = error.unpack_sent_trx();
            dtrx.delay_sec = 2;
            dtrx.send(_self.value, _self);
        }
        else{
            print("Txn couldnt take place");
        }
}
/*
// void mainloan::updatescore(name acc_name ,uint64_t credit_score,uint64_t status,
//                             uint64_t loan_instl, uint64_t loan_individual){
//
//       auto borrower = borr_table.get(acc_name.value);
//       eosio::check(borrower.acc_name==acc_name, "Borrower doesn't exist.");
//
//       //Update_credit_score function here
// }
// */
//
extern "C" void apply(uint64_t receiver, uint64_t code, uint64_t action){
  if (code=="eosio"_n.value && action=="onerror"_n.value){
    print("     Hello error came.    ");
    eosio::execute_action(eosio::name(receiver), eosio::name(code),
      &mainloan::onanerror);
      return;
  }
  else{
    switch(action){
      EOSIO_DISPATCH_HELPER(mainloan, (addborrower)(adduwr)(addloan)(getborrower)
                (getuwr)(defincr)(getloan)(addinstl)(sendinstl)(clearall)(getschedule)
                (checkpayment)(checkperiod)(notify))
    }
    eosio_exit(0);
  }
}


// ///namespace eosio
// EOSIO_DISPATCH(mainloan, (addborrower)(adduwr)(getborrower)(getuwr)(addloan)(getloan)(addinstl))
