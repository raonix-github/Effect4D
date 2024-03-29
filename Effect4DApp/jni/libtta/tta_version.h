#ifndef _TTA_VERSION_H_
#define _TTA_VERSION_H_

/* The major version, (1, if %LIBTTA_VERSION is 1.2.3) */
#define LIBTTA_VERSION_MAJOR (2)

/* The minor version (2, if %LIBTTA_VERSION is 1.2.3) */
#define LIBTTA_VERSION_MINOR (0)

/* The micro version (3, if %LIBTTA_VERSION is 1.2.3) */
#define LIBTTA_VERSION_MICRO (0)

/* The full version, like 1.2.3 */
#define LIBTTA_VERSION        2.0.0

/* The full version, in string form (suited for string concatenation)
 */
#define LIBTTA_VERSION_STRING "2.0.0"

/* Numerically encoded version, like 0x010203 */
#define LIBTTA_VERSION_HEX ((LIBTTA_MAJOR_VERSION << 24) |        \
                               (LIBTTA_MINOR_VERSION << 16) |        \
                               (LIBTTA_MICRO_VERSION << 8))

/* Evaluates to True if the version is greater than @major, @minor and @micro
 */
#define LIBTTA_VERSION_CHECK(major,minor,micro)      \
    (LIBTTA_VERSION_MAJOR > (major) ||               \
     (LIBTTA_VERSION_MAJOR == (major) &&             \
      LIBTTA_VERSION_MINOR > (minor)) ||             \
     (LIBTTA_VERSION_MAJOR == (major) &&             \
      LIBTTA_VERSION_MINOR == (minor) &&             \
      LIBTTA_VERSION_MICRO >= (micro)))

#endif /* _TTA_VERSION_H_ */
