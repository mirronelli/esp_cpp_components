# mTime

Ultrasmall time utility providing current time and timestring. When used, automatically syncs time with SNTP before providing value for the first time to prevent returning time out of sync.

sample:
```cpp
mTime::SetTimeZone("CET-1CEST,M3.5.0,M10.5.0/3");

string timeString = mTime::GetTimeString(mDateTime dateTime);

mDateTime date = mTime::GetTime();
time_t now = date.Now;
tm info = date.Info;
```