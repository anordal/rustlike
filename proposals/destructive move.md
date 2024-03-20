# A proposal for destructive move through function arguments in C++ and C

## Why

C++: Current move semantics does not move anything statically,
with all the safety and simplicity that leaves on the table:
Lifetime semantics.

C: Let existing APIs express their existing ownership semantics,
thereby making it possible to borrow-check calling code without any changes.
The innate requirement to call all destructors explicitly in all code paths makes C
uniquely positioned to adopt fully resource-linear types from under the rug.

Same solution for C and C++.

Rust stopped at resource-affine types. Let's not:
Having resource-linear types opens up for destructors that take arguments,
and when arguments have lifetime, this enforces correct destruction order
more elegantly than requiring objects to carry around references to each other
with lifetime annotations.

Stack unwinding + resource-linear types = a solved problem:
A `defer` guard (in the standard library) would
let the user set up lambdas to be called at scope exit.
With lifetime semantics, there is no risk of forgetting to use it.

## Theory

*Destructive move* means passing the ownership of an object from one variable to another,
where ownership is the responsibility to pass that ownerhip away.

The difference from what C++ calls "move" is that the source variable is considered destroyed
afterwards, not destroyed yet again at the end of its scope.
Thus, calling any function that overtakes the responsibility to eventually call the destructor
is as good as calling the destructor itself as far as satisfying the caller's responsibility.

In other words, variables have a static (type level) property of needing to be destroyed.
At each point in the program, the compiler considers it to be either invalid memory or alive:

    +---------+  transfer ownerhip to / begin ownership or lifetime / construct  +---------+
    | Invalid | ---------------------------------------------------------------> | Alive & |
    | memory  |  transfer ownership from / end ownership or lifetime / destruct  | Owned   |
    +---------+ <--------------------------------------------------------------- +---------+

Two implications of this state machine:

* No discrimination against method of state transition:
  To the caller, destructively moving out of a variable and destruction itself are one and the same.
  Likewise, so are destructively moving into a variable and construction itself.
  The difference in effect is up to the functions that implement these contractual state transitions.
  Constructors and destructors are just special cases of functions that give and take ownership of variables.
  As are two-sided moves, including where source and destination happen to be of the same type.
  Since it does not matter, let's call the two possible state transitions _construction_ and _destruciton_.
* Symmetry: Construction and destruction are just reverse opposites (consider the reverse program).
  All that can be said about properties of destruction can be said about construction too.

One such property:
Customarily, destruction can be implicit or explicit:

* If the type is *resource-affine*, which is to say that it has an implicitly callable destructor,
  the compiler is allowed to solve the responsibility problem by inserting implicit destructor calls
  in code paths where the user hasn't.
* If the type is *resource-linear*, meaning that it must likewise be destroyed, just not implicitly,
  compilation shall instead fail if the user failed to destroy it in every code path.

See [substructural type system – the resource interpretation](https://en.wikipedia.org/wiki/Substructural_type_system#The_resource_interpretation) on Wikipedia.

## Problem description

*The following is a type description, not a language description.*

```C
{
    // Declaration is not initialization, just the earliest opportunity to construct.
    // Construction is a state transition that we sometimes want to save for later.
    Bread bread;

    // Construction must be done exactly once in each code path.
    if (breadbox_has()) {
        breadbox_withdraw(&bread);
    } else {
        bread_bake(&bread);
    }

    // Borrowing is allowed between construction and destruction.
    take_picture(&bread);

    // Destruction must be done exactly once in each code path, explicitly or implicitly.
    if (edible(&bread)) {
        eat(&bread);
    } else {
        bread_destroy(&bread);
    }

    // End of scope is not destruction, just the latest opportunity to destruct.
    // Destruction is a state transition that we sometimes want to expedite earlier.
}
```

## The proposal says itself

A C++ specific solution would not solve the right problem:
All kinds of non-special functions need to transfer ownership,
which is not solvable by adding yet another constructor, operator
or other special function that gets its semantics by entitlement.

(That said, it makes sense for C++,
that has member functions, useful for upholding state invariants,
that the internal ability to be the ultimate sources and drains for objects of its type
be restricted to member functions. That's a secondary discussion.)

What is needed is a notation that applies independently to each function argument.
Specifically pointer and reference arguments.

(It is also worth pondering by-value arguments and return values à la Rust.
It would be good to add that too, but this proposal forgoes that for now,
because it can not solve all problems in the most ideal way.)

Qualifiers (akin to `const` and `volatile`) fit the bill without adding syntax.
For transferring ownership both from and to (effectively in-arguments and out-arguments),
2 new qualifiers are needed. That's the proposal.

Of the reserved keywords in C++, a pair that stands out is `new` and `delete`.
Take that as a suggestion, but let's run with it for the sake of demonstration.

```C
void buy_candy(new Candy *dst, delete Coin *src);
```
