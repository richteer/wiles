# Wiles
A fork of `richteer/fermat` targetting ARM instead of x86\_64.

## Dependencies
- a C compiler
- a lex equivalent (flex)
- a yacc equivalent (bison)
- tolerance for terrible error messages

Be sure to set CC, LEX, and YACC in the Makefile to match your appropriate utilities.

## Usage
Source text is inputted on `stdin` (likely to change in the future).
The optional `-v` flag can be used to display the scanning/parsing magic.
In fact, without it, it does basically nothing.

## Status

All of the existing bugs in `richtter/fermat` exist in this, including the following:
- Mysterious segfaul on entry, some weird ARM quirk I haven't figured out
- `ID` referencing hasn't been completely fleshed out yet (mostly the `spew_id` function)

What works:
- Can compile the provided example in `example/wat.fm`
- Can compile most non `ID`-requiring source

## Remarks
Admittedly, porting some of this to armv6 was not as easy as I expected.
Despite a lot of it resulting in essentially being find and replace, determining as new calling convention was difficult.
ARM and x86\_64 operate in different manners, thus a simple replace is not quite possible.
For example, there are 32 registers, r0, r1, .. r31, some with a specific purpose (like instruction pointer).
Secondly, instructions allow three arguments, take `add` for example:
```
add r0, r1, r2
```
adds `r1` and `r2` and stores the result in `r0`.
In x86\_64, this required clobbering one of the registers, where this allows preservation of the original.
Even still, for simplicity's and consistency's sake, I chose to adopt the convention of clobbering one of the registers anyway.
Another notable difference is the way branching works.
I am still trying to figure this one out, but the convenient instructions `call`, `leave`, `ret`, etc do not exist, thus required a bit more playing around to figure out a similar.

Overall, the result of a few hours of learning ARM assembly by looking at the generated assembly from GCC yielded a separate compiler that barely functions.
Targeting new platforms requires a bit of extra thought, more than I originally anticipated.

`http://www.github.com/richteer/wiles`

