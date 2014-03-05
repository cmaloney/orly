/* <base/chrono.cc>

   Implements <base/chrono.h>

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

#include <base/chrono.h>

#include <iostream>
#include <iomanip>
#include <sstream>

#include <base/convert.h>
#include <base/piece.h>
#include <base/syntax_error.h>

using namespace Base;
using namespace Base::Chrono;

/* Convenience macro to wrap the Read function. */
#define READ(part, delimiter) Read(converter, part, #part, delimiter)

/* TODO */
template <typename TVal>
static void Read(TConverter &converter, TVal &part, const char *name, const char delimiter) {
  assert(&converter);
  assert(&part);
  converter.Read(part);
  if (delimiter != '\0') {
    if (*converter != delimiter) {
      std::ostringstream oss;
      oss << name << " must be delimited by " << delimiter;
      throw TSyntaxError(HERE, oss.str().c_str());
    }
    ++converter;
  }
}

/* TODO */
namespace NumNanosecond {

  /* Was 1000 for milliseconds, changed to 1000000000 for Nano */
  constexpr uint64_t Second = 1000000000;

  /* TODO */
  constexpr uint64_t Minute = Second * 60;

  /* TODO */
  constexpr uint64_t Hour = Minute * 60;

  /* TODO */
  constexpr uint64_t Day = Hour * 24;

}  // NumSecond

/** Helper function to read nanoseconds
 */
uint64_t ReadNanoseconds(TConverter &converter) {

    ++converter;

    uint64_t Nanosecond = 0;
    uint64_t multiplier = 100000000;
    uint64_t digit_val = 0;

    while (converter.HasDigit()) {
      converter.TryReadDigit(digit_val);
      Nanosecond += digit_val * multiplier;

      if (multiplier == 1) {
	break;
      }
      multiplier /= 10;
    }

    return Nanosecond;
}

/**
 *   TTimeDiffInfo
 **/
TTimeDiffInfo::TTimeDiffInfo(
    bool is_forward,
    uint64_t day,
    uint64_t hour,
    uint64_t minute,
    uint64_t second,
    uint64_t nanosecond)
      : IsForward(is_forward),
        Day(day),
        Hour(hour),
        Minute(minute),
        Second(second),
	Nanosecond(nanosecond) {}

/* TODO */
TTimeDiffInfo::TTimeDiffInfo(const std::string &str) {
  assert(&str);
  TConverter converter(AsPiece(str));
  TOpt<bool> sign = converter.TryReadSign();
  IsForward = sign ? *sign : true;
  READ(Day, 'T');
  READ(Hour, ':');
  READ(Minute, ':');
  READ(Second, '\0');
  if (converter) {
    Nanosecond = ReadNanoseconds(converter);
  }
  else {
    Nanosecond = 0;
  }
}

/* TODO */
TTimeDiffInfo::TTimeDiffInfo(const TTimeDiff &time_diff)
    : Nanosecond(time_diff.count()) {
  assert(&time_diff);
  IsForward = (Nanosecond >= 0);
  Day = Nanosecond / NumNanosecond::Day;
  Nanosecond %= NumNanosecond::Day;
  Hour = Nanosecond / NumNanosecond::Hour;
  Nanosecond %= NumNanosecond::Hour;
  Minute = Nanosecond / NumNanosecond::Minute;
  Nanosecond %= NumNanosecond::Minute;
  Second = Nanosecond / NumNanosecond::Second;
  Nanosecond %= NumNanosecond::Second;
}

/* TODO */
std::string TTimeDiffInfo::AsString() const {
  assert(this);
  std::ostringstream strm;
  Write(strm);
  return strm.str();
}

/* TODO */
TTimeDiff TTimeDiffInfo::AsTimeDiff() const {
  assert(this);
  return TTimeDiff((Day * NumNanosecond::Day +
                    Hour * NumNanosecond::Hour +
                    Minute * NumNanosecond::Minute +
                    Second * NumNanosecond::Second +
		    Nanosecond) * (IsForward ? 1 : -1));
}

/* TODO */
void TTimeDiffInfo::Write(std::ostream &strm) const {
  assert(this);
  assert(&strm);
  strm
    << (IsForward ? '+' : '-')
    << Day << 'T'
    << Hour << ':'
    << Minute << ':'
    << Second;
  if (Nanosecond > 0) {
    strm << '.' << std::setw(9) << std::setfill ('0') << Nanosecond;
  }
}

/**
 *   TTimePntInfo
 **/
TTimePntInfo::TTimePntInfo(
    uint64_t year, uint64_t month, uint64_t day,
    uint64_t hour, uint64_t minute, uint64_t second,
    uint64_t nanosecond, int64_t utc_offset)
      : Year(year), Month(month), Day(day),
        Hour(hour), Minute(minute), Second(second),
        Nanosecond(nanosecond), UtcOffset(utc_offset) {
  Validate(HERE);
}

/* TODO */
TTimePntInfo::TTimePntInfo(const std::string &str)
    : UtcOffset(0) {
  assert(&str);
  TConverter converter(AsPiece(str));
  READ(Year, '-');
  READ(Month, '-');
  READ(Day, 'T');
  READ(Hour, ':');
  READ(Minute, ':');
  READ(Second, '\0');
  if (*converter == '.') {
    Nanosecond = ReadNanoseconds(converter);
  }
  else {
    Nanosecond = 0;
  }

  // utc offset
  int64_t hour_offset = 0, minute_offset = 0;
  if (*converter != 'Z') {
    bool is_forward = converter.ReadSign();
    READ(hour_offset, ':');
    READ(minute_offset, '\0');
    UtcOffset = (hour_offset * 60 + minute_offset) * (is_forward ? 1 : -1);
  }
  Validate(HERE);
}

/* Construct a TimePntObj from TTimePnt.
   TTimePnt -> to_time_t() -> time_t -> gmtime() -> std::tm
   then use info in std::tm to construct */
TTimePntInfo::TTimePntInfo(const TTimePnt &time_pnt)
    : UtcOffset(0) { // Always go to zulu time
  assert(&time_pnt);

  /* std::tm doesn't support nanoseconds so will need to add something */
  auto ms = time_pnt.time_since_epoch().count();
  Nanosecond = ms % 1000000000;

  /* time_t doesn't want to deal with nanoseconds so let's try it with seconds since
     that's all we use anyway */
  auto sec_time_pnt = std::chrono::time_point_cast<std::chrono::seconds>(time_pnt);

  time_t temp = TClock::to_time_t(sec_time_pnt);
  const std::tm &tm_obj = *std::gmtime(&temp);
  Year = 1900 + tm_obj.tm_year;
  Month = tm_obj.tm_mon + 1;
  Day = tm_obj.tm_mday;
  Hour = tm_obj.tm_hour;
  Minute = tm_obj.tm_min;
  Second = tm_obj.tm_sec;

  Validate(HERE);
}

/* TODO */
std::string TTimePntInfo::AsString() const {
  assert(this);
  std::ostringstream strm;
  Write(strm);
  return strm.str();
}

/* TODO */
TTimePnt TTimePntInfo::AsTimePnt() const {
  assert(this);

  std::tm tm_obj;
  time_t zulu_time = timegm(&(AsTmObj(tm_obj)));
  if (zulu_time == -1) {
    throw TChronoError(HERE, "parameters to the time_pnt contructor are invalid");
  }
  return TimePntCast(TClock::from_time_t(zulu_time)) + std::chrono::nanoseconds(Nanosecond);
}

/* Convert a TimePntObj to a std::tm
   NOTE. This is more of a helper function for AsTimePnt */
std::tm &TTimePntInfo::AsTmObj(std::tm &out) const {
  assert(this);
  out.tm_year = Year - 1900;
  out.tm_mon = Month - 1;
  out.tm_mday = Day;
  out.tm_hour = Hour;
  out.tm_min = Minute;
  out.tm_sec = Second;
  out.tm_hour -= UtcOffset / 60;
  out.tm_min -= UtcOffset % 60;
  return out;
}

/* TODO */
void TTimePntInfo::Validate(const TCodeLocation &here) const {
  assert(this);
  if (Year < 1900) {
    throw TChronoError(here, "Year prior to 1900 is not supported");
  }
  if (Month < 1 || 12 < Month) {
    throw TChronoError(here, "Month must be between 1 and 12");
  }
  if (Day < 1 || 31 < Day) {
    throw TChronoError(here, "Day must be between 1 and 31");
  }
  if (Hour < 0 || 23 < Hour) {
    throw TChronoError(here, "Hour must be between 0 and 23");
  }
  if (Minute < 0 || 59 < Minute) {
    throw TChronoError(here, "Minute must be between 0 and 59");
  }
  if (Second < 0 || 60 < Second) {
    throw TChronoError(here, "Second must be between 0 and 60");
  }
  if (Nanosecond < 0 || 999999999 < Nanosecond) {
    throw TChronoError(here, "Nanosecond must be between 0 and 999999999");
  }
  if (UtcOffset < -720 || 840 < UtcOffset) {
    throw TChronoError(here, "UtcOffset must be between -720 and 840");
  }
}

/* TODO */
void TTimePntInfo::Write(std::ostream &strm) const {
  assert(this);
  assert(&strm);
  strm
    << Year << '-'
    << Month << '-'
    << Day << 'T'
    << Hour << ':'
    << Minute << ':'
    << Second;
  if (Nanosecond > 0) {
    strm << '.' << std::setw(9) << std::setfill ('0') << Nanosecond;
  }
  if (UtcOffset == 0) {
    strm << 'Z';
  } else {
    if (UtcOffset > 0) {
      strm << '+';
    } else if (UtcOffset < 0) {
      strm << '-';
    }
    strm << UtcOffset;
  }

}

/**
 *   Utility functions
 **/
TTimeDiff Base::Chrono::CreateTimeDiff(
    bool is_forward,
    uint64_t day,
    uint64_t hour,
    uint64_t minute,
    uint64_t second,
    uint64_t nanosecond) {
  return TTimeDiffInfo(is_forward, day, hour, minute, second, nanosecond).AsTimeDiff();
}

/* TODO */
TTimePnt Base::Chrono::CreateTimePnt(
    uint64_t year, uint64_t month, uint64_t day,
    uint64_t hour, uint64_t minute, uint64_t second,
    uint64_t nanosecond, int64_t utc_offset) {
  return TTimePntInfo(year, month, day, hour, minute, second, nanosecond, utc_offset).AsTimePnt();
}

/* TODO */
TTimePnt Base::Chrono::Now() { return TimePntCast(TClock::now()); }