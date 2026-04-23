// time comprehensive tests
import "time" for Time, Duration, Timer

// Test Time construction from Unix timestamp
var t1 = Time.fromUnix(0)
System.print(t1.sec == 0)  // expect: true

var t2 = Time.fromUnix(1000000)
System.print(t2.sec == 1000000)  // expect: true

// Test Time from components
var t3 = Time.new(2024, 1, 15, 10, 30, 0)
System.print(t3.year == 2024)  // expect: true
System.print(t3.month == 1)  // expect: true
System.print(t3.day == 15)  // expect: true
System.print(t3.hour == 10)  // expect: true
System.print(t3.minute == 30)  // expect: true
System.print(t3.second == 0)  // expect: true

// Test Time comparison
var tA = Time.fromUnix(100)
var tB = Time.fromUnix(200)
System.print(tA < tB == true)  // expect: true
System.print(tA > tB == false)  // expect: true
System.print(tA == tA == true)  // expect: true

// Test Time arithmetic with Duration
var tBase = Time.fromUnix(1000)
var d1 = Duration.new(100)
var tFuture = tBase.add(d1)
System.print(tFuture.sec == 1100)  // expect: true

var tPast = tBase.sub(d1)
System.print(tPast.sec == 900)  // expect: true

// Test Duration between two times
var diff = tB.sub(tA)
System.print(diff.seconds == 100)  // expect: true

// Test Duration construction and properties
var dur = Duration.hours(2)
System.print(dur.seconds == 7200)  // expect: true
System.print(dur.milliseconds == 7200000)  // expect: true

var dur2 = Duration.minutes(30)
System.print(dur2.seconds == 1800)  // expect: true
System.print(dur2.mins == 30)  // expect: true

var dur3 = Duration.seconds(90)
System.print(dur3.secs == 30)  // expect: true
System.print(dur3.mins == 1.5)  // expect: true

// Test Duration arithmetic
var dSum = Duration.minutes(10) + Duration.seconds(30)
System.print(dSum.seconds == 630)  // expect: true

var dDiff = Duration.minutes(10) - Duration.minutes(3)
System.print(dDiff.seconds == 420)  // expect: true

var dMul = Duration.seconds(10) * 3
System.print(dMul.seconds == 30)  // expect: true

var dDiv = Duration.seconds(60) / 4
System.print(dDiv.seconds == 15)  // expect: true

// Test Duration comparison
var dSmall = Duration.seconds(10)
var dLarge = Duration.seconds(20)
System.print(dSmall < dLarge == true)  // expect: true
System.print(dSmall > dLarge == false)  // expect: true
System.print(dSmall == dSmall == true)  // expect: true

// Test Duration abs
var dNeg = Duration.new(-100)
System.print(dNeg.abs.seconds == 100)  // expect: true

// Test Timer
var timer = Timer.new()
timer.start()
Time.sleep(0.01)  // 10ms
timer.stop()
System.print(timer.elapsed >= 0.01)  // expect: true

// Test Timer lap
var timer2 = Timer.new()
timer2.start()
Time.sleep(0.01)
var lap1 = timer2.lap()
System.print(lap1 >= 0.01)  // expect: true

// Test Duration common values
System.print(Duration.zero.seconds == 0)  // expect: true
System.print(Duration.second.seconds == 1)  // expect: true
System.print(Duration.minute.seconds == 60)  // expect: true
System.print(Duration.hour.seconds == 3600)  // expect: true
System.print(Duration.day.seconds == 86400)  // expect: true
System.print(Duration.week.seconds == 604800)  // expect: true

// Test Time toString (just check it returns a string)
var tStr = t3.toString
System.print(tStr is String)  // expect: true
System.print(tStr.contains("2024"))  // expect: true
