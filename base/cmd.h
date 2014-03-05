/* <base/cmd.h>

   Parse command line parameters.

      // Inherit from Base::TCmd
      class TCircle : public Base::TCmd {
        public:
        // Provide defaults for optional arguments.
        TCircle() : IsFilled(false) {}
        private:
        // Inherit from Base::TCmd::TMeta to describe yourself.
        class TMeta
            : public TCmd::TMeta {
          public:
          TMeta()
              : Base::TMeta("Draw a circle.") {  // Construct the base with a descriptive string.
            // Declare your parameters.
            Param(&TCircle::IsFilled, "is_filled", Optional, "filled\0", "The circle should be drawn filled-in.");
            Param(&TCircle::Radius, "radius", Required, "The radius of the circle.");
          }
        };
        // Override this function to check argument values, if needed.
        virtual bool CheckArgs(const TMeta::TMessageConsumer &cb) {
          if (Radius < 0 && !cb("must be non-negative")) {
            return false;
          }
          return true;
        }
        // Override this function to return an instance of your meta-object.
        virtual const Base::TMeta *NewMeta() const {
          return new TMeta();
        }
        // Override this function to do the work of your command.
        virtual int Run() {
          // draw a circle, maybe?
          return EXIT_SUCCESS;
        }
        // Declare backing storage for your arguments.
        bool IsFilled;
        double Radius;
      };

      // Implement 'main' like this:
      int main(int argc, char *argv[]) {
        return TCircle().Main(argc, argv);
      }

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

#include <cassert>
#include <functional>
#include <istream>
#include <mutex>
#include <ostream>
#include <set>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <getopt.h>

#include <base/demangle.h>
#include <base/no_copy_semantics.h>
#include <base/no_construction.h>
#include <base/thrower.h>

namespace Base {

  /* The base class for all commands to be handled from the command line. */
  class TCmd {
    NO_COPY_SEMANTICS(TCmd);
    public:

    /* The directory in which the executable resides.  Not null.  No trailing slash.
       Taken from argv[0]. */
    const char *GetDir() const {
      assert(this);
      return Dir;
    }

    /* The name of the executable.
       Taken from argv[0]. */
    const char *GetProg() const {
      assert(this);
      return Prog;
    }

    protected:

    /* The base class for all meta-objects describing command objects. */
    class TMeta {
      NO_COPY_SEMANTICS(TMeta);
      public:

      /* A function which receives error messages which arise during command line parsing
         and/or argument consistency checking.  */
      typedef std::function<bool (const std::string &)> TMessageConsumer;

      /* Do-little. */
      virtual ~TMeta();

      /* Dump the arguments of the command object to the stream.
         Useful for debugging. */
      void Dump(std::ostream &strm, const TCmd *cmd) const;

      /* Parse the command line, writing errors to stderr.
         Return the number of errors encountered. */
      size_t Parse(TCmd *cmd, int argc, char *argv[]) const;

      /* Parse the command line, calling back for each error message. */
      bool Parse(TCmd *cmd, int argc, char *argv[], const TMessageConsumer &cb) const;

      /* Override this function to write whatever you want after the command description.
         The default implementation writes nothing. */
      virtual void WriteAfterDesc(std::ostream &strm) const;

      /* Write the help text, like this:

           usage: <prog> pos0 pos1..

           <desc>

           pos0  help...
           pos1  help...

           OPTIONS:
             --opt1  help...
             --opt2  help...
      */
      void WriteHelp(std::ostream &strm, const TCmd *cmd, size_t line_size = 0, size_t pad_size = 2) const;

      /* Write the message to stderr and increment the count.
         Return true always. */
      static bool WriteToStdErr(const std::string &msg, size_t &count);

      protected:

      /* Is a parameter required or optional? */
      enum TBasis {

        /* The parameter must be provided.
           If the parameter is multi-valued, the minimum number of values is one. */
        Required,

        /* The parameter is optional.
           If the parameter is multi-valued, the minimum number of values is zero. */
        Optional

      };  // TBasis

      /* Does a parameter occur at most once or many times? */
      enum TRecurrence {

        /* The paramater may occur at most once. */
        Once,

        /* The paramater may occur many times, possibly with duplicate values. */
        Many,

        /* The paramater may occur many times, but values may not be duplicated. */
        NoDupes

      };  // TRecurrence

      /* Do-little. */
      TMeta(const char *desc = 0);

      /* Declare a pass-by-name parameter with a backer. */
      template <typename TSomeCmd, typename TVal>
      void Param(TVal (TSomeCmd::*backer), const char *arg_name, TBasis basis, const char *names, const char *desc) {
        assert(this);
        AppendParam(new TBackedParam<TSomeCmd, TVal>(backer, arg_name, basis, names, desc));
      }

      /* Declare a pass-by-position parameter with a backer. */
      template <typename TSomeCmd, typename TVal>
      void Param(TVal (TSomeCmd::*backer), const char *arg_name, TBasis basis, const char *desc) {
        assert(this);
        AppendParam(new TBackedParam<TSomeCmd, TVal>(backer, arg_name, basis, 0, desc));
      }

      /* Declare a pass-by-name parameter without a backer. */
      template <typename TSomeCmd>
      void Param(bool (TSomeCmd::*handler)(const TMessageConsumer &), TBasis basis, const char *names, const char *desc) {
        assert(this);
        AppendParam(new TUnbackedParam<TSomeCmd>(handler, basis, names, desc));
      }

      private:

      /* The base class for all command line parameter meta-objects. */
      class TParam {
        NO_COPY_SEMANTICS(TParam);
        public:

        /* The base class for all argument meta-objects.
           Here use 'argument' to refer to the value being parsed by the parameter. */
        class TArg {
          NO_COPY_SEMANTICS(TArg);
          public:

          /* True iff. this argument is required. */
          virtual bool GetIsRequired() const = 0;

          /* See TRecurrence. */
          virtual TRecurrence GetRecurrence() const = 0;

          /* The name of this argument. */
          const char *GetName() const {
            assert(this);
            return Name;
          }

          /* Parse a new value for this argument from the text.
             If the text is null, assign the argument a default value.
             Note: The text should not be null if GetIsRequired() returns true. */
          bool Parse(TCmd *cmd, const char *arg_text, const TMessageConsumer &cb) const;

          /* Write the current value of this argument. */
          virtual void Write(std::ostream &strm, const TCmd *cmd) const = 0;

          /* Write the type of this argument. */
          virtual void WriteType(std::ostream &strm) const = 0;

          protected:

          /* A helper in which to look up information about value types.
             This generic version handles non-container types without default state
             and which have standard stream insertion and extraction.
             Make explicit specializations to handle type-specific behaviors. */
          template <typename TVal>
          class ValInfo {
            NO_CONSTRUCTION(ValInfo);
            public:

            /* A non-container value can occur at most once. */
            static TRecurrence GetRecurrence() {
              return Once;
            }

            /* Read a value from a stream. */
            static void Read(std::istream &strm, TVal &val) {
              assert(&strm);
              assert(&val);
              strm >> val;
            }

            /* Most values have no default state. */
            static const TVal *TryGetDefault() {
              return 0;
            }

            /* Write a value to a stream. */
            static void Write(std::ostream &strm, const TVal &val) {
              assert(&strm);
              assert(&val);
              strm << val;
            }

            /* Write a human-readable description of the value type. */
            static void WriteType(std::ostream &strm) {
              assert(&strm);
              strm << TDemangle(typeid(TVal)).Get();
            }

          };  // ValInfo<TVal>

          /* Do-little. */
          TArg(const char *name)
              : Name(name) {
            assert(name);
          }

          /* Do-little. */
          virtual ~TArg() {}

          /* Assign to the argument the default value for its type. */
          virtual void AssignDefault(TCmd *cmd) const = 0;

          /* Assign to the argument a value read from the stream. */
          virtual void Read(std::istream &strm, TCmd *cmd) const = 0;

          private:

          /* Write the contents of the container as a comma-separated list. */
          template <typename TContainer>
          static void WriteCsl(std::ostream &strm, const TContainer &container) {
            assert(&strm);
            assert(&container);
            bool sep_flag = true;
            for (const typename TContainer::value_type &val: container) {
              if (sep_flag) {
                strm << ", ";
              } else {
                sep_flag = true;
              }
              strm << val;
            }
          }

          /* See accessor. */
          const char *Name;

        };  // TArg

        /* Do-little. */
        virtual ~TParam();

        /* Call back for each name by which this parameter may be passed.
           If this is a pass-by-position paramater, don't call back at all and just return true. */
        bool ForEachName(const std::function<bool (const char *)> &cb) const;

        /* The argument accepted by this parameter.  If this parameter as no argument, calling this function is an error.
           Note that a pass-by-position parameter always has an argument, and a pass-by-name parameter may or may not have. */
        const TArg *GetArg() const {
          assert(this);
          auto arg = TryGetArg();
          assert(arg);
          return arg;
        }

        /* See TBasis. */
        TBasis GetBasis() const {
          assert(this);
          return Basis;
        }

        /* A name by which to refer to this parameter in diagnostics.  Never null. */
        const char *GetDiagnosticName() const {
          assert(this);
          return Names ? Names : GetArg()->GetName();
        }

        /* True iff. this is a pass-by-name parameter. */
        bool GetIsByName() const {
          assert(this);
          return Names != 0;
        }

        /* See TRecurrence. */
        TRecurrence GetRecurrence() const {
          assert(this);
          auto arg = TryGetArg();
          return arg ? arg->GetRecurrence() : Once;
        }

        /* True if at least one of our names is the same as the given name. */
        bool Matches(const char *target_name) const;

        /* Be notified that this parameter has been recognized on the command line.
           The 'arg_text' can be null if the command line didn't provide any such text.
           Call back with errors, and short-circuit of the callback says to do so. */
        virtual bool OnRecognition(TCmd *cmd, const char *arg_text, const TMessageConsumer &cb) const = 0;

        /* Return the argument delegate, if any.
           A parameter which takes no argument will return null. */
        virtual const TArg *TryGetArg() const = 0;

        /* Write the type of cmd which uses this parameter. */
        virtual void WriteCmd(std::ostream &strm) const = 0;

        /* Write the help text for this parameter. */
        void WriteHelp(std::ostream &strm, const TCmd *cmd) const;

        /* Write the command line item for this parameter.
           A pass-by-name parameter will write nothing. */
        void WriteLineItem(std::ostream &strm) const;

        protected:

        /* The base class for all argument meta-objects of a given type. */
        template <typename TVal>
        class TTypedArg
            : public TArg {
          NO_COPY_SEMANTICS(TTypedArg);
          public:

          /* See base class. */
          virtual bool GetIsRequired() const {
            assert(this);
            return ValInfo<TVal>::TryGetDefault() == 0;
          }

          /* See base class. */
          virtual TRecurrence GetRecurrence() const {
            return ValInfo<TVal>::GetRecurrence();
          }

          /* See base class. */
          virtual void Write(std::ostream &strm, const TCmd *cmd) const {
            assert(this);
            ValInfo<TVal>::Write(strm, GetVal(cmd));
          }

          /* Write the type of this argument. */
          virtual void WriteType(std::ostream &strm) const {
            ValInfo<TVal>::WriteType(strm);
          }

          protected:

          /* Do-little. */
          TTypedArg(const char *name)
              : TArg(name) {}

          /* A reference to the argument within the command object. */
          virtual const TVal &GetVal(const TCmd *cmd) const = 0;

          /* A reference to the argument within the command object. */
          virtual TVal &GetVal(TCmd *cmd) const = 0;

          private:

          /* See base class. */
          virtual void AssignDefault(TCmd *cmd) const {
            assert(this);
            auto default_val = ValInfo<TVal>::TryGetDefault();
            assert(default_val);
            GetVal(cmd) = *default_val;
          }

          /* See base class. */
          virtual void Read(std::istream &strm, TCmd *cmd) const {
            assert(this);
            ValInfo<TVal>::Read(strm, GetVal(cmd));
          }

        };  // TTypedArg<TVal>

        /* Do-little. */
        TParam(TBasis basis, const char *names, const char *desc)
            : Basis(basis), Names(names), Desc(desc) {
          assert(desc);
        }

        private:

        /* Write help a piece at a time, calling back before each piece. */
        bool ForEachHelpPiece(std::ostream &strm, const TCmd *cmd, const std::function<bool ()> &cb) const;

        /* See TBasis. */
        TBasis Basis;

        /* A multistring containing the names by which this parameter may be passed.
           If this is a pass-by-position parameter, this will be null. */
        const char *Names;

        /* A helpful description of the parameter.  Never null. */
        const char *Desc;

      };  // TParam

      /* A parameter that is backed by storage in a TCmd object. */
      template <typename TSomeCmd, typename TVal>
      class TBackedParam
          : public TParam {
        NO_COPY_SEMANTICS(TBackedParam);
        public:

        /* The type of our backer. */
        typedef TVal (TSomeCmd::*TBacker);

        /* Do-little. */
        TBackedParam(TBacker backer, const char *arg_name, TBasis basis, const char *names, const char *desc)
            : TParam(basis, names, desc), Arg(backer, arg_name) {}

        /* See base class. */
        virtual bool OnRecognition(TCmd *cmd, const char *arg_text, const TMessageConsumer &cb) const {
          assert(this);
          return Arg.Parse(cmd, arg_text, cb);
        }

        /* See base class. */
        virtual const TParam::TArg *TryGetArg() const {
          assert(this);
          return &Arg;
        }

        /* See base class. */
        virtual void WriteCmd(std::ostream &strm) const {
          assert(&strm);
          strm << TDemangle(typeid(TSomeCmd)).Get();
        }

        private:

        /* An argument accepted by this parameter. */
        class TArg
            : public TParam::TTypedArg<TVal> {
          NO_COPY_SEMANTICS(TArg);
          public:

          /* Do-little. */
          TArg(TBacker backer, const char *name)
              : TTypedArg<TVal>(name), Backer(backer) {}

          private:

          /* See base class. */
          virtual const TVal &GetVal(const TCmd *cmd) const {
            assert(this);
            assert(cmd);
            const TSomeCmd *some_cmd = dynamic_cast<const TSomeCmd *>(cmd);
            assert(some_cmd);
            return some_cmd->*Backer;
          }

          /* See base class. */
          virtual TVal &GetVal(TCmd *cmd) const {
            assert(this);
            assert(cmd);
            TSomeCmd *some_cmd = dynamic_cast<TSomeCmd *>(cmd);
            assert(some_cmd);
            return some_cmd->*Backer;
          }

          /* Our backer within the command object. */
          TBacker Backer;

        };  // TArg

        /* A delegate for handling argument-specific behavior. */
        TArg Arg;

      };  // TBackedParam<TSomeCmd, TVal>

      /* A parameter this is not backed by storage in a TCmd object. */
      template <typename TSomeCmd>
      class TUnbackedParam
          : public TParam {
        NO_COPY_SEMANTICS(TUnbackedParam);
        public:

        /* The type of our handler. */
        typedef bool (TSomeCmd::*THandler)(const TMessageConsumer &cb);

        /* Construct as a pass-by-name parameter. */
        TUnbackedParam(THandler handler, TBasis basis, const char *names, const char *desc)
            : TParam(basis, names, desc), Handler(handler) {}

        /* See base class. */
        virtual bool OnRecognition(TCmd *cmd, const char *, const TMessageConsumer &cb) const {
          assert(this);
          return (cmd->*Handler)(cb);
        }

        /* See base class. */
        virtual const TArg *TryGetArg() const {
          return 0;
        }

        /* See base class. */
        virtual void WriteCmd(std::ostream &strm) const {
          assert(&strm);
          strm << TDemangle(typeid(TSomeCmd)).Get();
        }

        private:

        /* Our handler within the command object. */
        THandler Handler;

      };  // TUnbackedParam<TSomeCmd, TVal>

      /* Append the parameter to our array of parameters.
         Also check for consistency errors, raising exceptions as appropriate. */
      void AppendParam(TParam *param);

      /* Write col1 and col2 to the stream as wrapped columns of text.
         Make each line no longer than line_size. */
      static void WriteWrapped(std::ostream &strm, size_t col1_size, size_t line_size, const std::string &col1, const std::string &col2);

      /* A description of the parameter. */
      const char *Desc;

      /* If true, the last pass-by-position parameter is optional and/or repeatable. */
      bool AmbigPosEnd;

      /* The parameters. */
      std::vector<TParam *> Params;

      /* All the names used by our pass-by-name parameters. */
      std::unordered_set<std::string> UsedNames;

      public:

      /* A parser for the command line.
         This is basically a big wrapper around getopt_long_only(). */
      class TParser {
        NO_COPY_SEMANTICS(TParser);
        public:

        /* A parser for the parameters described by the given meta-object. */
        TParser(const TMeta *meta);

        /* Parse argc/argv into the given command object.
           Call back with errors, aborting if requested. */
        bool Parse(TCmd *cmd, int argc, char *argv[], const TMessageConsumer &cb) const {
          return TRun()(this, cmd, argc, argv, cb);
        }

        /* Return the param matching the given name, if any. */
        const TParam *TryFindParam(const char *name) const;

        private:

        /* A run of the parser. */
        class TRun {
          NO_COPY_SEMANTICS(TRun);
          public:

          /* Do-little. */
          TRun() {}

          /* Parse. */
          bool operator()(const TParser *parser, TCmd *cmd, int argc, char *argv[], const TMessageConsumer &cb);

          private:

          /* Call back for any missing but required parameters. */
          bool CheckForRequired(const std::vector<const TParam *> &params, const TMessageConsumer &cb) const;

          /* Count this recognition of the parameter, calling back if we violate the recurrence rule for it.
             Otherwise, deletgate to the parameter's OnRecognition() action. */
          bool OnRecognition(const TParam *param, TCmd *cmd, const char *arg_text, const TMessageConsumer &cb);

          /* The parameters we've seen and the number of times we've seen them. */
          std::unordered_map<const TParam *, size_t> CountByParam;

          /* Used to prevent reentry to getopt_long_only(). */
          static std::mutex Mutex;

        };  // TRun

        /* The argument for getopt_long_only(). */
        std::vector<option> Options;

        /* The parameters we're going to parse, separated into by-name and by-position groups. */
        std::vector<const TParam *> ByName, ByPos;

        /* Used to disambiguate the result of getopt_long_only(). */
        static const int Offset;

        /* Used to make insertion into TRun::CountByParam more convenient. */
        static const size_t Zero;

      };  // TParser

    };  // TMeta

    /* Do-little. */
    TCmd()
        : IsHelp(false), Dir("<dir>"), Prog("<prog>") {}

    /* Do-little. */
    virtual ~TCmd();

    /* Override this to perform consistency checks on your arguments.
       Call back with errors, aborting if requested. */
    virtual bool CheckArgs(const TMeta::TMessageConsumer &cb);

    /* Use the given meta object to parse the command line and check for argument consistency.
       If the command line contained errors, write them to stderr and exit the process with EXIT_FAILURE;
       if help was requested, write it to stdout and exit the process with EXIT_SUCCESS;
       otherwise, just return. */
    void Parse(int argc, char *argv[], const TMeta &meta);

    private:

    /* See accessor. */
    bool IsHelp;

    /* See accessors. */
    const char *Dir, *Prog;

    /* See accessor. */
    std::vector<std::string> ArgsFiles;

  };  // TCmd

  /* Explicit specialization of TCmd::TMeta::TParam::TArg::ValInfo<>
     for bool arguments. */
  template <>
  class TCmd::TMeta::TParam::TArg::ValInfo<bool> {
    NO_CONSTRUCTION(ValInfo);
    public:

    static TRecurrence GetRecurrence() {
      return Once;
    }

    static void Read(std::istream &strm, bool &val) {
      assert(&strm);
      assert(&val);
      strm >> std::boolalpha >> val;
    }

    static const bool *TryGetDefault() {
      return &Default;
    }

    static void Write(std::ostream &strm, const bool &val) {
      assert(&strm);
      assert(&val);
      strm << std::boolalpha << val;
    }

    static void WriteType(std::ostream &strm) {
      assert(&strm);
      strm << "bool";
    }

    private:

    static const bool Default;

  };  // TCmd::TMeta::TParam::TArg::ValInfo<bool>

  /* Explicit specialization of TCmd::TMeta::TParam::TArg::ValInfo<>
     for std::string arguments. */
  template <>
  class TCmd::TMeta::TParam::TArg::ValInfo<std::string> {
    NO_CONSTRUCTION(ValInfo);
    public:

    static TRecurrence GetRecurrence() {
      return Once;
    }

    static void Read(std::istream &strm, std::string &val) {
      assert(&strm);
      assert(&val);
      strm >> val;
    }

    static const std::string *TryGetDefault() {
      return 0;
    }

    static void Write(std::ostream &strm, const std::string &val) {
      assert(&strm);
      assert(&val);
      strm << val;
    }

    static void WriteType(std::ostream &strm) {
      assert(&strm);
      strm << "string";
    }

  };  // TCmd::TMeta::TParam::TArg::ValInfo<std::string>

  /* Explicit specialization of TCmd::TMeta::TParam::TArg::ValInfo<>
     for vector arguments. */
  template <typename TVal>
  class TCmd::TMeta::TParam::TArg::ValInfo<std::vector<TVal>> {
    NO_CONSTRUCTION(ValInfo);
    public:

    static TRecurrence GetRecurrence() {
      return Many;
    }

    static void Read(std::istream &strm, std::vector<TVal> &vals) {
      assert(&strm);
      assert(&vals);
      TVal val;
      ValInfo<TVal>::Read(strm, val);
      vals.push_back(val);
    }

    static const std::vector<TVal> *TryGetDefault() {
      return 0;
    }

    static void Write(std::ostream &strm, const std::vector<TVal> &vals) {
      WriteCsl(strm, vals);
    }

    static void WriteType(std::ostream &strm) {
      assert(&strm);
      strm << "array of ";
      ValInfo<TVal>::WriteType(strm);
    }

  };  // TCmd::TMeta::TParam::TArg::ValInfo<std::vector<TVal>>

  /* Explicit specialization of TCmd::TMeta::TParam::TArg::ValInfo<>
     for set arguments. */
  template <typename TVal>
  class TCmd::TMeta::TParam::TArg::ValInfo<std::set<TVal>> {
    NO_CONSTRUCTION(ValInfo);
    public:

    static TRecurrence GetRecurrence() {
      return NoDupes;
    }

    static void Read(std::istream &strm, std::set<TVal> &vals) {
      DEFINE_ERROR(error_t, std::invalid_argument, "duplicate value");
      assert(&strm);
      assert(&vals);
      TVal val;
      ValInfo<TVal>::Read(strm, val);
      if (!vals.insert(val).second) {
        THROW_ERROR(error_t) << '"' << val << '"';
      }
    }

    static const std::set<TVal> *TryGetDefault() {
      return 0;
    }

    static void Write(std::ostream &strm, const std::set<TVal> &vals) {
      WriteCsl(strm, vals);
    }

    static void WriteType(std::ostream &strm) {
      assert(&strm);
      strm << "set of ";
      ValInfo<TVal>::WriteType(strm);
    }

  };  // TCmd::TMeta::TParam::TArg::ValInfo<std::set<TVal>>

}  // Base
