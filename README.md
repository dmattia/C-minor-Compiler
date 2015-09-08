# C-minor-Compiler
A Compiler created using C for the C-minor language

The C-minor language is a made up language for my compilers course.  C-minor is a simplified version of C with a different looking syntax.  

In C-minor, variables are declared as:
```
x: integer = 65;
y: char = 'A';
```
and functions are declared as:
```
printarray: function void ( a: array [] integer, size: integer ) = {
	i: integer;
	for( i=0;i<size;i++) {
		print a[i], "\n";
	}
}
```
Other differences to C are that C-minor is strictly typed. As far as types go, it adds the bool type and removes all floating point values.
