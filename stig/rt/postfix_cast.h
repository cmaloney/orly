/* <stig/rt/postfix_cast.h>

   TODO

   Copyright 2010-2014 Stig LLC

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#pragma once

#include <climits>
#include <cmath>
#include <sstream>
#include <string>
#include <utility>

#include <base/chrono.h>
#include <base/no_construction.h>
#include <stig/rt/containers.h>
#include <stig/rt/generator.h>
#include <stig/rt/mutable.h>
#include <stig/rt/opt.h>
#include <stig/rt/runtime_error.h>
#include <stig/uuid.h>

namespace Stig {

  namespace Rt {

    /* TODO */
    template <typename TTo, typename TFrom>
    struct CastAs {
      NO_CONSTRUCTION(CastAs);

      /* TODO */
      static TTo Do(const TFrom &val) = delete;

    };  // CastAs

    /* TODO */
    template <typename TVal>
    struct CastAs<TVal, TVal> {
      NO_CONSTRUCTION(CastAs);

      /* TODO */
      static const TVal &Do(const TVal &val) {
        return val;
      }

    };  // CastAs<TVal, TVal>

    /* TODO */
    template <>
    struct CastAs<double, int64_t> {
      NO_CONSTRUCTION(CastAs);

      /* TODO */
      static double Do(const int64_t &val) {
        return static_cast<double>(val);
      }

    };  // CastAs<double, int64_t>

    /* TODO */
    template <>
    struct CastAs<int64_t, double> {
      NO_CONSTRUCTION(CastAs);

      /* TODO */
      static int64_t Do(const double &val) {
        return static_cast<int64_t>(val);
      }

    };  // CastAs<int64_t, double>

    /* TODO */
    template <>
    struct CastAs<std::string, int64_t> {
      NO_CONSTRUCTION(CastAs);

      /* TODO */
      static std::string Do(const int64_t &val) {
        std::ostringstream oss;
        oss << val;
        if (oss.fail()) {
          throw TSystemError(HERE, "Cast from int to string failed.");
        }
        return oss.str();
      }

    };  // CastAs<std::string, int64_t>

    /* TODO */
    template <>
    struct CastAs<int64_t, std::string> {
      NO_CONSTRUCTION(CastAs);

      /* TODO */
      static int64_t Do(const std::string &val) {
        char *endptr = 0;
        int64_t n = strtol(val.c_str(), &endptr, 10);
        if (endptr != &val.c_str()[val.size()]) {
          throw TSystemError(HERE, "Part of the string is not convertible");
        }
        if (errno == ERANGE && (n == LONG_MAX || n == LONG_MIN)) {
          throw TSystemError(HERE, "integer type exceeds limit");
        }
        return n;
      }

    };  // CastAs<int64_t, std::string>

    /* TODO */
    template <>
    struct CastAs<std::string, double> {
      NO_CONSTRUCTION(CastAs);

      /* TODO */
      static std::string Do(const double &val) {
        std::stringstream oss;
        oss << std::showpoint << val;
        if (oss.fail()) {
          throw TSystemError(HERE, "Cast from real to string failed.");
        }
        return oss.str();
      }

    };  // CastAs<std::string, double>

    /* TODO */
    template <>
    struct CastAs<double, std::string> {
      NO_CONSTRUCTION(CastAs);

      /* TODO */
      static double Do(const std::string &val) {
        char *endptr = 0;
        double d = strtod(val.c_str(), &endptr);
        if (endptr != &val.c_str()[val.size()]) {
          throw TSystemError(HERE, "Part of the string is not convertible");
        }
        if (errno == ERANGE) {
          if (d == HUGE_VALF || d == HUGE_VALL) {
            throw TSystemError(HERE, "real type overflows limit");
          } else if (d == 0) {
            throw TSystemError(HERE, "real type underflows limit");
          }
        }
        return d;
      }

    };  // CastAs<double, std::string>

    /* TODO */
    template <>
    struct CastAs<TUUID, std::string> {
      NO_CONSTRUCTION(CastAs);

      /* TODO */
      static TUUID Do(const std::string &val) {
        return TUUID(val.c_str());
      }

    };  // CastAs<double, int64_t>


    /* TODO */
    template <>
    struct CastAs<std::string, TUUID> {
      NO_CONSTRUCTION(CastAs);

      /* TODO */
      static std::string Do(const TUUID &val) {
        assert(&val);
        return val.AsString();
      }

    };  // CastAs<double, int64_t>

    /* TODO */
    template <typename TVal>
    struct CastAs<Rt::TOpt<TVal>, TVal> {
      NO_CONSTRUCTION(CastAs);

      /* TODO */
      static Rt::TOpt<TVal> Do(const TVal &val) {
        return Rt::TOpt<TVal>(val);
      }

    };  // CastAs<Rt::TOpt<TVal>, TVal>


    /* TODO */
    template <typename TTo, typename TFrom>
    struct CastAs<std::vector<TTo>, TFrom> {
      NO_CONSTRUCTION(CastAs);

      /* TODO */
      static std::vector<TTo> Do(const TFrom &val) {
        return {CastAs<TTo, TFrom>::Do(val)};
      }

    };  // CastAs<std::vector<TTo>, TFrom>

    /* TODO */
    template <typename TVal>
    struct CastAs<std::vector<TVal>, std::vector<TVal>> {
      NO_CONSTRUCTION(CastAs);

      /* TODO */
      static const std::vector<TVal> &Do(const std::vector<TVal> &val) {
        return val;
      }

    };  // CastAs<std::vector<TVal>, std::vector<TVal>>

    /* TODO */
    template <typename TTo, typename TFrom>
    struct CastAs<std::vector<TTo>, std::vector<TFrom>> {
      NO_CONSTRUCTION(CastAs);

      /* TODO */
      static std::vector<TTo> Do(const std::vector<TFrom> &from) {
        std::vector<TTo> to;
        for (auto elem : from) {
          to.push_back(CastAs<TTo, TFrom>::Do(elem));
        }
        return to;
      }

    };  // CastAs<std::vector<TTo>, std::vector<TFrom>>

    /* TODO */
    template <typename TTo, typename TFrom>
    struct CastAs<std::vector<TTo>, std::shared_ptr<const Rt::TGenerator<TFrom>>> {
      NO_CONSTRUCTION(CastAs);

      /* TODO */
      static std::vector<TTo> Do(const typename Rt::TGenerator<TFrom>::TPtr &val) {
        std::vector<TTo> to;
        for(auto cursor = val->NewCursor(); cursor; ++cursor) {
          to.push_back(CastAs<TTo, TFrom>::Do(*cursor));
        }
        return to;
      }

    };  // CastAs<std::vector<TVal>, Rt::TGenerator<TFrom>

    /* TODO */
    template <typename TVal>
    struct CastAs<TSet<TVal>, TSet<TVal>> {
      NO_CONSTRUCTION(CastAs);

      /* TODO */
      static const TSet<TVal> &Do(const TSet<TVal> &val) {
        return val;
      }

    };  // CastAs<TSet<TVal>, TSet<TVal>>

    /* TODO */
    template <typename TTo, typename TFrom>
    struct CastAs<TSet<TTo>, TSet<TFrom>> {
      NO_CONSTRUCTION(CastAs);

      /* TODO */
      static TSet<TTo> Do(const TSet<TFrom> &from) {
        TSet<TTo> to;
        for (auto elem : from) {
          to.insert(CastAs<TTo, TFrom>::Do(elem));
        }
        return to;
      }

    };  // CastAs<TSet<TTo>, TSet<TFrom>>

    /* TODO */
    template <typename TTo, typename TFrom>
    struct CastAs<TSet<TTo>, std::shared_ptr<const Rt::TGenerator<TFrom>>> {
      NO_CONSTRUCTION(CastAs);

      /* TODO */
      static TSet<TTo> Do(const typename Rt::TGenerator<TFrom>::TPtr &val) {
        TSet<TTo> to;
        for(auto cursor = val->NewCursor(); cursor; ++cursor) {
          to.insert(CastAs<TTo, TFrom>::Do(*cursor));
        }
        return to;
      }

    };  // CastAs<TSet<TVal>, Rt::TGenerator<TFrom>

    /* TODO */
    template <typename TKey, typename TVal>
    struct CastAs<TDict<TKey, TVal>, TDict<TKey, TVal>> {
      NO_CONSTRUCTION(CastAs);

      /* TODO */
      static const TDict<TKey, TVal> &Do(const TDict<TKey, TVal> &val) {
        return val;
      }

    };  // CastAs<TDict<TKey, TVal>, TDict<TKey, TVal>>

    /* TODO */
    template <typename TToKey, typename TToVal, typename TFromKey, typename TFromVal>
    struct CastAs<TDict<TToKey, TToVal>, TDict<TFromKey, TFromVal>> {
      NO_CONSTRUCTION(CastAs);

      /* TODO */
      static TDict<TToKey, TToVal> Do(const TDict<TFromKey, TFromVal> &from) {
        TDict<TToKey, TToVal> to;
        for (auto elem : from) {
          to.insert(std::make_pair(CastAs<TToKey, TFromKey>::Do(elem.first),
                              CastAs<TToVal, TFromVal>::Do(elem.second)));
        }
        return to;
      }

    };  // CastAs<TDict<TToKey, TToVal>, TDict<TFromKey, TFromVal>>

    /* TODO */
    template <typename TToKey, typename TToVal, typename TFromKey, typename TFromVal>
    struct CastAs<TDict<TToKey, TToVal>, std::tuple<TFromKey, TFromVal>> {
      NO_CONSTRUCTION(CastAs);

      typedef std::tuple<TFromKey, TFromVal> TFinalTuple;

      /* TODO */
      static TDict<TToKey, TToVal> Do(const TFinalTuple &from) {
        return TDict<TToKey, TToVal>{{CastAs<TToKey, TFromKey>::Do(/*from.template Get<0>()*/std::get<0>(from)),
            CastAs<TToVal, TFromVal>::Do(/*from.template Get<1>()*/std::get<1>(from))}};
      }

    };  // CastAs<TDict<TToKey, TToVal>, TDict<TFromKey, TFromVal>>

    /* TODO */
    template <typename TToKey, typename TToVal, typename TFromKey, typename TFromVal>
    struct CastAs<TDict<TToKey, TToVal>,
          std::shared_ptr<const Rt::TGenerator<std::tuple<TFromKey, TFromVal>>>> {
      NO_CONSTRUCTION(CastAs);

      typedef std::tuple<TFromKey, TFromVal> TFinalTuple;
      typedef std::shared_ptr<const Rt::TGenerator<TFinalTuple>> TTupleGen;

      /* TODO */
      static TDict<TToKey, TToVal> Do(const TTupleGen &from) {
        TDict<TToKey, TToVal> to;
        for(auto it = from->NewCursor(); it; ++it) {
          //TODO: A += would make this SOOO much more efficient.
          to = to + CastAs<TDict<TToKey, TToVal>, TFinalTuple>::Do(*it);
        }
        return to;
      }

    };  // CastAs<TDict<TToKey, TToVal>, TDict<TFromKey, TFromVal>>

    template <typename TVal, typename TAddr>
    struct CastAs<TVal, TMutable<TAddr, TVal>> {
      NO_CONSTRUCTION(CastAs);

      static TVal Do(const TMutable<TAddr, TVal> &from) {
        assert(&from);
        return from;
      }

    };  // CastAs<TVal, TMutable<TAddr, TVal>>

    template <>
    struct CastAs<std::string, Base::Chrono::TTimeDiff> {
      NO_CONSTRUCTION(CastAs);

      static std::string Do(const Base::Chrono::TTimeDiff &from) {
        return Base::Chrono::TTimeDiffInfo(from).AsString();
      }

    };  // CastAs<std::string, Base::Chrono::TTimeDiff>

    template <>
    struct CastAs<std::string, Base::Chrono::TTimePnt> {
      NO_CONSTRUCTION(CastAs);

      static std::string Do(const Base::Chrono::TTimePnt &from) {
        return Base::Chrono::TTimePntInfo(from).AsString();
      }

    };  // CastAs<std::string, Base::Chrono::TTimePnt>

    template <>
    struct CastAs<Base::Chrono::TTimeDiff, std::string> {
      NO_CONSTRUCTION(CastAs);

      static Base::Chrono::TTimeDiff Do(const std::string &from) {
        return Base::Chrono::TTimeDiffInfo(from).AsTimeDiff();
      }

    };  // CastAs<Base::Chrono::TTimeDiff, std::string>

    template <>
    struct CastAs<Base::Chrono::TTimePnt, std::string> {
      NO_CONSTRUCTION(CastAs);

      static Base::Chrono::TTimePnt Do(const std::string &from) {
        return Base::Chrono::TTimePntInfo(from).AsTimePnt();
      }

    };  // CastAs<Base::Chrono::TTimePnt, std::string>

  }  // Rt

}  // Stig
