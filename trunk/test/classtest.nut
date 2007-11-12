class HelloWorld {
  constructor() {
    test <- "HelloWorld";
  }
  
  function print() {
    ::print(test);
  }
  
};

print("--------------------");
a <- HelloWorld();
a.foo <- "Test"
a.print();

/* EOF */
