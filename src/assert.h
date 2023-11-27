/*
 * src/assert.h
 *
 * Contain macros that assert various facts about Values for error detection
 * and handling purposes.
 *
 */
#ifndef lye_assert_h
#define lye_assert_h

/*
 * src/assert.h:ASSERT
 * buildyourownlisp.com correspondence: LASSERT
 *
 * The base assertion, handling the actual condition and return on error.
 *
 */
#define ASSERT(value, condition, format, ...)                                  \
  if (!(condition)) {                                                          \
    Value *error = make_error(format, ##__VA_ARGS__);                          \
    delete_value(value);                                                       \
    return error;                                                              \
  }

#define BASE_FORMAT "function '%s' must be passed %s"

/*
 * src/assert.h:ASSERT_ARGC
 * buildyourownlisp.com correspondence: LASSERT_NUM
 *
 * Assert that the Function Value was called with the correct number of
 * arguments.
 *
 */
#define ASSERT_ARGC(value, expected_argc, caller)                              \
  do {                                                                         \
    int actual_argc = count(value);                                            \
    char *plural = expected_argc == 1 ? "" : "s";                              \
    ASSERT(value, actual_argc == expected_argc,                                \
           "function '%s' must be passed %d argument%s, but got %d instead.",  \
           caller, expected_argc, plural, actual_argc)                         \
  } while (0)

/*
 * src/assert.h:ASSERT_IS_NUMBER
 * buildyourownlisp.com correspondence: LASSERT_TYPE(LVAL_NUM)
 *
 * Assert that the Value is of type Number.
 *
 */
#define ASSERT_IS_NUMBER(value, index, caller)                                 \
  do {                                                                         \
    ASSERT(                                                                    \
        value, IS_NUMBER(element_at(value, index)),                            \
        "operator '%s' can only operate on numbers. Found value of type %s.",  \
        caller, get_type(value))                                               \
  } while (0)

/*
 * src/assert.h:ASSERT_IS_LIST
 * buildyourownlisp.com correspondence: LASSERT_TYPE(LVAL_QEXPR)
 *
 * Assert that the Value is of type Q-Expression (List).
 *
 */
#define ASSERT_IS_LIST(value, index, caller)                                   \
  do {                                                                         \
    ASSERT(value, IS_QEXPR(element_at(value, index)), BASE_FORMAT, caller,     \
           "a list.")                                                          \
  } while (0)

/*
 * src/assert.h:ASSERT_IS_SYMBOL
 * buildyourownlisp.com correspondence: LASSERT_TYPE(LVAL_SYM)
 *
 * Assert that the Value is of type Symbol.
 *
 */
#define ASSERT_IS_SYMBOL(value, element, caller)                               \
  do {                                                                         \
    ASSERT(value, IS_SYMBOL(element), BASE_FORMAT, caller,                     \
           "a list of symbols.")                                               \
  } while (0)

/*
 * src/assert.h:ASSERT_CONTAINS_VALUES
 * buildyourownlisp.com correspondence: LASSERT_NOT_EMPTY
 *
 * Assert that the Value is a nonempty List.
 *
 */
#define ASSERT_CONTAINS_VALUES(value, caller)                                  \
  do {                                                                         \
    ASSERT(value, count(element_at(value, 0)) > 0, BASE_FORMAT, caller,        \
           "a list with at least one element.")                                \
  } while (0)

#endif