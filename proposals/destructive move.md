# A proposal for destructive move through function arguments in C++ and C

## Definition

Destructive move means to transfer ownership, where ownership is the responsibility to pass that ownership on (such as to a destructor).

See also Wikipedia: [Substructural type system § The resource interpretation](https://en.wikipedia.org/wiki/Substructural_type_system#The_resource_interpretation)

## Goals

### Actually transfer ownership

C++: Current move semantics does not move anything statically, with all the performance and static verifiability that leaves on the table.

C: Let existing APIs express their existing ownership semantics, thereby making it possible to borrow-check calling code without any changes. The innate requirement to call all destructors explicitly in all code paths makes C uniquely positioned to adopt fully resource-linear types from under the rug.

### Same solution for C and C++

C++: Note that adding yet another speical member function or operator is no solution at all, because all kinds of non-special functions also need to be able to transfer ownership.

### Resource-linear types, both from and to

Rust lacks these.

Observe the symmentry that memory is uninitialized (and must not be read from or initialized) before being moved to, just like it is indeterminate (and must not be read from or destroyed) after being moved from.

C++: Making the destructor uncallable to the user of an object (by any means, such as by making it private) would have the effect of forcing the user to call any other function that fits the bill, which can ultimately forward it to the destructor.

## The proposal says itself

What is needed is a per-function-argument notation for pointer and reference arguments.

Qualifiers (like `const` and `volatile`) fit the bill without adding syntax.

For transferring ownership both from and to (in other words, both in-arguments and out-arguments), 2 new qualifiers are needed. That's the proposal.

Of the reserved keywords in C++, a pair that stands out is `new` and `delete`. That's merely a suggestion, but for sake of example, let's continue with that.

```C
void buy_milk(new Milk *dst, delete Coin *src);

Coin coin = …
Milk *carton = malloc(sizeof(Milk)); // get uninitialized memory
if(carton == NULL) … // error handling omitted

buy_milk(carton, &coin);
```