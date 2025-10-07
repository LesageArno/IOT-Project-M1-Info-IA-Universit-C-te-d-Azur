import jsonschema, json, sys
import jsonschema.exceptions as jsonExc

schema = {
    "type": "object",
    "properties": {
        "status": {
            "type": "object",
            "properties": {
                "temperature": {"type": "number", "minimum": -70, "maximum": 150},
                "light": {"type": "integer", "minimum": 0, "maximum": 4095},
                "regul": {"type": "string"},
                "fire": {"type": "boolean"},
                "heat": {"type": "string", "enum": ["ON", "OFF"]},
                "cold": {"type": "string", "enum": ["ON", "OFF"]},
                "fanspeed": {"type": "integer", "minimum": 0, "maximum": 255},   
            },
            "required": ["temperature", "light", "regul", "fire", "heat", "cold", "fanspeed"],
            "additionalProperties": False
        },
        
        "location": {
            "type": "object",
            "properties": {
                "room": {"type": "string"},
                "gps": {
                    "type": "object",
                    "properties": {
                        "lat": {"type": "number"},
                        "lon": {"type": "number"}
                    },
                    "required": ["lat", "lon"],
                    "additionalProperties": False
                },
                "address": {"type": "string"}
            },
            "required": ["room", "gps", "address"],
            "additionalProperties": False
        },
        
        "regul": {
            "type": "object",
            "properties": {
                "lt": {"type": "number"},
                "ht": {"type": "number"}
            },
            "required": ["lt", "ht"],
            "additionalProperties": False
        },
        
        "info": {
            "type": "object",
            "properties": {
                "ident": {"type": "string"},
                "user": {"type": "string"},
                "loc": {"type": "string"}
            },
            "required": ["ident", "user", "loc"],
            "additionalProperties": False
        },
        
        "net": {
            "type": "object",
            "properties": {
                "uptime": {"type": "string"},
                "ssid": {"type": "string"},
                "mac": {"type": "string"},
                "ip": {"type": "string"}
            },
            "required": ["uptime", "ssid", "mac", "ip"],
            "additionalProperties": False
        },
        
        "reporthost": {
            "type": "object",
            "properties": {
                "target_ip": {"type": "string"},
                "target_port": {"type": "integer"},
                "sp": {"type": "integer"}
            },
            "required": ["target_ip", "target_port", "sp"],
            "additionalProperties": False
        },     
    },
    "required": ["status", "location", "regul", "info", "net", "reporthost"],
    "additionalProperties": False
}


try:
    if "--fromfile" == sys.argv[1]:
        with open(sys.argv[2], "r") as file:
            inJson = json.load(file)
    else:
        inJson = json.loads(sys.argv[1])
    jsonschema.validate(inJson, schema)
except json.decoder.JSONDecodeError:
    print("ERROR 001: decoding JSON failed.")
    exit()
except jsonExc.ValidationError:
    print("ERROR 002: JSON format invalid.")
    exit()
except jsonExc.SchemaError:
    print("ERROR 003: Invalid JSON schema.")
    exit()
except IndexError:
    print("ERROR 004: Invalid argument")
    exit()
except FileNotFoundError:
    print("ERROR 005: File not found.")
    exit()
print("OK")