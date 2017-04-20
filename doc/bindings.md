
const
-----

Ruby does not know of const like C++ does. Unfortunately this enables ruby
programmers to do nasty things e.g. to ```const string&```. E.g.:

```
print sda.name, "\n"

sda.name = "oh no"

print sda.name, "\n"
```

If you do this kind of thing it is your fault. You have been warned!

