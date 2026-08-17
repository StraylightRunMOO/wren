import "time" for Time, Duration

var t = Time.fromUnix(0)
System.print(t.sec == 0)  // expect: true
System.print(Duration.hour.seconds == 3600)  // expect: true
var later = Time.fromUnix(10).add(Duration.seconds(5))
System.print(later.sec == 15)  // expect: true
System.print(Time.fromUnix(1) < Time.fromUnix(2))  // expect: true
