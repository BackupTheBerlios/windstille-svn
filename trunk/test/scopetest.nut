function scopetest() {
  {
    local a = 5;
    b <- 5;
  }
  // print(a + "\n");
  print(b + "\n");
}

scopetest();

/* EOF */
