var done = false
while (!done) {
  fn {
    continue // expect error
  }
  done = true
}