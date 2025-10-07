# Master Informatique - IA - IOT 2025 - Delivery 1
### ***Authors:** Arno LESAGE, Nathan THIERY* 

#### **How to use the validator**
The validator can be used directly via the commandline with the command:

    python .\validator.py [json] [--fromfile <path>]

It can be used directly by inserting a JSON within the command as in:

    python .\validator.py '{"status":{"temperature":23.5,"light":4095,"regul":"HALT","fire":false,"heat":"OFF","cold":"OFF","fanspeed":0},"location":{"room":"282","gps":{"lat":43.61708414,"lon":7.06471621},"address":"Les lucioles"},"regul":{"lt":26,"ht":25.89999962},"info":{"ident":"ESP32 123","user":"AL","loc":"A Biot"},"net":{"uptime":"55","ssid":"Livebox-B870","mac":"AC:67:B2:37:C9:48","ip":"192.168.1.45"},"reporthost":{"target_ip":"127.0.0.1","target_port":1880,"sp":2}}'

or via a path using the flag `--fromfile` as in:

    python .\validator.py --fromfile .\tests\test_valid.json

The constraint of the validator includes:
- All expected properties must appears,
- No unexpected properties must appears,
- The `temperature` must be contained within the range $[-70, 150]$,
- The `light` must be contained within the range $[\![0, 4095]\!]$,
- The `fanspeed` must be contained within the range $[\![0, 255]\!]$,
- The properties `heat` and `cold` are either `OFF` or `ON`,
- The type of each properties must be correct. 