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
a.print();

/* EOF */
