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
                "regul": {"type": "string", "maxLength": 150},
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
                "room": {"type": "string", "maxLength": 150},
                "gps": {
                    "type": "object",
                    "properties": {
                        "lat": {"type": "number"},
                        "lon": {"type": "number"}
                    },
                    "required": ["lat", "lon"],
                    "additionalProperties": False
                },
                "address": {"type": "string", "maxLength": 150}
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
                "ident": {"type": "string", "maxLength": 150},
                "user": {"type": "string", "maxLength": 150},
                "loc": {"type": "string", "maxLength": 150}
            },
            "required": ["ident", "user", "loc"],
            "additionalProperties": False
        },
        
        "net": {
            "type": "object",
            "properties": {
                "uptime": {"type": "string", "maxLength": 50},
                "ssid": {"type": "string", "maxLength": 150},
                "mac": {"type": "string", "maxLength": 50},
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

# Function to check validity of IP
def checkIPFormat(val:str) -> None:
    if len(val) > 15:
        raise jsonExc.ValidationError("Too long IP")
    try:
        val = [(int(i) >= 0 and int(i) <= 255) for i in val.split(".")]
        if not all(val) or len(val)!=4:
            raise jsonExc.ValidationError("Values in IP are not within [0, 255]")
    except:
        raise jsonExc.ValidationError("Values in IP are not number")






try:
    # Retrieve datas
    if "--fromfile" == sys.argv[1]:
        with open(sys.argv[2], "r") as file:
            inJson = json.load(file)
    else:
        inJson = json.loads(sys.argv[1])
        
    # Validate JSON
    jsonschema.validate(inJson, schema)
    
    # Check IP
    checkIPFormat(inJson["net"]["ip"])
    checkIPFormat(inJson["reporthost"]["target_ip"])
    
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
    
# If evrything's fine
print("OK")