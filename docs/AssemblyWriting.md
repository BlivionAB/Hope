### Assembly Writing

Since, Mach-O defines the code section before the constant section. We would have
to relocate some of the code.

So the steps are:
```c++
struct Constant 
{
    List<std::size_t>
    relocationAddress;
};

struct CString : Constant
{
    char*
    value;
};
```
* Write code
    * Write the instructions as they are defined. But, as soon as we encounter a 
      constant operator. We would have to add it to a relocation address field 
* Write referenced constants.
    * As we are writing the constants, we keep track of a running offset, to know 
      which address we are writing the constant on. When we have written the 
      constant, we will relocate the address used in the code, using the relocation
      address stored on the constant.