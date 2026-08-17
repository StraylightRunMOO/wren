// os - Operating system interface
// Provides access to standard streams, process args, environment, and exit.
// Standard streams are raw file descriptors; use io.reader()/io.writer() to wrap them.

class os {
  // Standard file descriptors
  static stdin  { 0 }
  static stdout { 1 }
  static stderr { 2 }

  // Command-line arguments (list of strings)
  static args { args_() }

  // Environment variable lookup; returns null if not set
  static env(key) { env_(key) }

  // Terminate the process
  static exit(code) { exit_(code) }

  foreign static args_()
  foreign static env_(key)
  foreign static exit_(code)
}

// Convenience aliases — typed as raw fd integers
var Stdin  = 0
var Stdout = 1
var Stderr = 2
