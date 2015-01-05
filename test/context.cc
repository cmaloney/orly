#include <test/context.h>

#include <stack>

#pragma clang diagnostic ignored "-Wexit-time-destructors"

using namespace Test;

TContext::TContext(const char *name) : Name(name), Parent(Current()) {
  // Put context onto the stack.
  Push(this);

  if(Name) {
    GetWriteTarget() << "begin " << Name << '\n';
  }
}

TContext::~TContext() {
  if(Name) {
    GetWriteTarget() << "end " << Name << "; " << (FailureCount ? "fail" : "pass") << '\n';
  }

  // Take this context off the stack
  TContext *ctx = Pop();
  assert(this == ctx);
  assert(Current() == Parent);

  if(Parent) {
    // Write to the parent the fixture result
    TContext::Log(Name, FailureCount > 0);
  }
}

TContext::TLog::~TLog() { *this << "\n"; }

TContext::TLog::TLog(const Base::TCodeLocation &loc, const std::string &&Test, bool passed)
    : TLog(passed) {
  *this << Base::AsStr(loc, Test, "; ");
}

TContext::TLog::TLog(bool passed) : Passed(passed), Print(!passed || GetOptions().Verbose) {
  Current()->LogResult(passed);
}

static std::stack<TContext *> &GetContextVector() {
  static std::stack<TContext *> Contexts;
  assert(&Contexts);
  return Contexts;
}

// Maintainging the current context stack
void TContext::Push(TContext *ctx) { GetContextVector().push(ctx); }
TContext *TContext::Pop() {
  TContext *result = Current();

  GetContextVector().pop();

  return result;
}

TContext *TContext::Current() {
  auto &st = GetContextVector();
  assert(&st);
  if(st.empty()) {
    return nullptr;
  } else {
    return st.top();
  }
}

std::ostream &TContext::GetWriteTarget() {
  if(WriteDirect || GetOptions().Verbose) {
    return std::cout;
  } else {
    return BuffWriter;
  }
}

void TContext::LogResult(bool pass) {
  if(pass) {
    ++PassCount;
  } else {
    WriteDirect = true;
    std::cout << BuffWriter.str();
    BuffWriter.str("");
    ++FailureCount;
  }
}