function varg_test(...) {
  print("Number of arguments: " + vargc + "\n");
  //for(local i = 0; i < vargc; ++i) {
  print(vargv);
}

varg_test("Hello World", 3, 5 ,6);
quit();

/* EOF */




