/* <orly/data/twitter_query.cc>

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <iostream>

#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include <base/log.h>
#include <base/sigma_calc.h>
#include <base/timer.h>
#include <orly/protocol.h>
#include <orly/client/client.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Socket;
using namespace Orly;
using namespace Client;

class TExerciseClient final
   : public TClient {
 public:

 TExerciseClient(const Base::TOpt<TUuid> &session_id, const TAddress &addr)
     : TClient(addr, session_id, seconds(600)) {}

 private:

 virtual void OnPovFailed(const Base::TUuid &/*repo_id*/) override {}

 virtual void OnUpdateAccepted(const Base::TUuid &/*repo_id*/, const Base::TUuid &/*tracking_id*/) override {}

 virtual void OnUpdateReplicated(const Base::TUuid &/*repo_id*/, const Base::TUuid &/*tracking_id*/) override {}

 virtual void OnUpdateDurable(const Base::TUuid &/*repo_id*/, const Base::TUuid &/*tracking_id*/) override {}

 virtual void OnUpdateSemiDurable(const Base::TUuid &/*repo_id*/, const Base::TUuid &/*tracking_id*/) override {}

};

/* Command-line arguments. */
class TCmd final
   : public Base::TLog::TCmd {
 public:

 /* Construct with defaults. */
 TCmd() : Addr(TAddress::IPv4Loopback, DefaultPortNumber) {}

 /* Construct from argc/argv. */
 TCmd(int argc, char *argv[])
     : TCmd() {
   Parse(argc, argv, TMeta());
 }

 /* TODO  */
 Socket::TAddress Addr;

 private:

 /* Our meta-type. */
 class TMeta final
     : public Base::TLog::TCmd::TMeta {
   public:

   /* Registers our fields. */
   TMeta()
       : Base::TLog::TCmd::TMeta("The twitter exercise.") {
     Param(
         &TCmd::Addr, "addr", Optional, "addr\0",
         "Address of the server."
     );
   }

 };  // TCmd::TMeta

};  // TCmd

void GetUsers(const ::TCmd &cmd) {
 void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
 try {
   Base::TOpt<TUuid> session_id;
   auto client = make_shared<TExerciseClient>(session_id, cmd.Addr);
   auto pov_id = client->NewFastPrivatePov(TOpt<TUuid>::GetUnknown(), seconds(10));
   Base::TUuid id_to_use = **pov_id;
   std::set<int64_t> user_set;
   /* twitter.1.users */ {
     Base::TTimer timer;
     timer.Start();
     /* read */ {
       auto read_result = client->Try(id_to_use, { "twitter" }, TClosure(string("users")));
       Sabot::ToNative(*Sabot::State::TAny::TWrapper((*read_result)->GetValue().NewState((*read_result)->GetArena().get(), state_alloc)), user_set);
     }
     timer.Stop();
     stringstream ss;
     ss << "Users function returned [" << user_set.size() << "] users in time [" << timer.Total() << "s]" << std::endl;
     std::cout << ss.str();
   }
   /* twitter.1.total_tweet_count */ {
     int64_t out;
     Base::TTimer timer;
     timer.Start();
     /* read */ {
       auto read_result = client->Try(id_to_use, { "twitter" }, TClosure(string("total_tweet_count")));
       Sabot::ToNative(*Sabot::State::TAny::TWrapper((*read_result)->GetValue().NewState((*read_result)->GetArena().get(), state_alloc)), out);
     }
     timer.Stop();
     stringstream ss;
     ss << "Total Tweet Count function returned [" << out << "] tweets in time [" << timer.Total() << "s]" << std::endl;
     std::cout << ss.str();
   }
   /* twitter.1.tweet_count */ {
     Base::TSigmaCalc tweet_count_calc;
     std::list<std::shared_ptr<Rpc::TFuture<TMethodResult>>> outstanding_reqs;
     int64_t out;
     Base::TTimer timer;
     timer.Start();
     for (int64_t uid : user_set) {
       /* read */ {
         if (outstanding_reqs.size() >= 500) {
           const auto &read_res = outstanding_reqs.front();
           Sabot::ToNative(*Sabot::State::TAny::TWrapper((*read_res)->GetValue().NewState((*read_res)->GetArena().get(), state_alloc)), out);
           tweet_count_calc.Push(out);
           outstanding_reqs.pop_front();
         }
         outstanding_reqs.emplace_back(client->Try(id_to_use, { "twitter" }, TClosure(string("tweet_count"),
                                                                                      string("uid"), uid)));
       }
     }
     for (const auto &read_res : outstanding_reqs) {
       Sabot::ToNative(*Sabot::State::TAny::TWrapper((*read_res)->GetValue().NewState((*read_res)->GetArena().get(), state_alloc)), out);
       tweet_count_calc.Push(out);
     }
     timer.Stop();
     stringstream ss;
     ss << "Tweet Count Per User function [" << tweet_count_calc << "] in [" << timer.Total() << "]s\t[" << (user_set.size() / timer.Total()) << "]qps" << std::endl;
     std::cout << ss.str();
   }
   /* twitter.1.responded_to */ {
     Base::TSigmaCalc tweet_count_calc;
     std::list<std::shared_ptr<Rpc::TFuture<TMethodResult>>> outstanding_reqs;
     std::set<int64_t> out;
     Base::TTimer timer;
     timer.Start();
     for (int64_t uid : user_set) {
       /* read */ {
         if (outstanding_reqs.size() >= 500) {
           const auto &read_res = outstanding_reqs.front();
           Sabot::ToNative(*Sabot::State::TAny::TWrapper((*read_res)->GetValue().NewState((*read_res)->GetArena().get(), state_alloc)), out);
           tweet_count_calc.Push(out.size());
           outstanding_reqs.pop_front();
         }
         outstanding_reqs.emplace_back(client->Try(id_to_use, { "twitter" }, TClosure(string("responded_to"),
                                                                                      string("uid"), uid)));
       }
     }
     for (const auto &read_res : outstanding_reqs) {
       Sabot::ToNative(*Sabot::State::TAny::TWrapper((*read_res)->GetValue().NewState((*read_res)->GetArena().get(), state_alloc)), out);
       tweet_count_calc.Push(out.size());
     }
     timer.Stop();
     stringstream ss;
     ss << "Responded Count Per User function [" << tweet_count_calc << "] in [" << timer.Total() << "]s\t[" << (user_set.size() / timer.Total()) << "]qps" << std::endl;
     std::cout << ss.str();
   }
 } catch (const std::exception &ex) {
   std::cerr << ex.what() << std::endl;
 }
 std::cout << "Done" << std::endl;
}

int main(int argc, char *argv[]) {
 ::TCmd cmd(argc, argv);
 TLog log(cmd);

 /* install twitter.1 */ {
   auto client = make_shared<TExerciseClient>(TOpt<TUuid>::GetUnknown(), cmd.Addr);
   auto ack = client->InstallPackage({ "twitter" }, 1);
   ack->Sync();
 }
 GetUsers(cmd);
}
