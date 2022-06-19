/* PATHS */
#define PATH_ROOT               ("/")
#define PATH_PRINT_SUMMARY      ("/printSummary")
#define PATH_RING               ("/ring")
#define PATH_MAILBOX            ("/mailbox")
#define PATH_SENSOR             ("/sensor")
#define PATH_SENSOR_MEASURE     ("/measure")

/* KEYS of the '/ring' path query string*/
#define WEB_KEY_RINGTYPE        ("ringtype")
#define WEB_KEY_COLORCODE       ("ccode")

/* VALUES of the '/ring' path query string*/
#define WEB_VAL_BLINK           ("blink")
#define WEB_VAL_WAVE            ("wave")


/* KEYS of the '/mailbox' path query string, KEYS for the '/sensor' path query string */
#define WEB_KEY_ACTION          ("action")

/* Values of the '/mailbox' path query string*/
#define WEB_VALUE_OPEN          ("open")
#define WEB_VALUE_CLOSE         ("close")

/* Values of the '/sensor' path query string*/
#define WEB_VALUE_MEASURELS     ("measureLS")
#define WEB_VALUE_MEASUREUS     ("measureUS")
#define WEB_VALUE_CLEARLS       ("clearLS")
#define WEB_VALUE_CLEARUS       ("clearUS")

/* KEYS of the /printSummary path query string */
#define WEB_KEY_TASK            ("task")

/* Values of the /printSummary path query string */
#define WEB_VAL_WEBSERVER       ("webserver")
#define WEB_VAL_SENSOR          ("sensor")
#define WEB_VAL_SMARTDOOR       ("smartDoor")