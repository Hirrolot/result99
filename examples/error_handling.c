// A simple CSV parser to demonstrate context-preserving error handling with sum types.

#include <result99.h>

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

typedef const char *Path;
typedef void (*Callback)(size_t i, size_t j, const char *value);

// clang-format off
datatype(
    ParseCsvRecordErrorKind,
    (ParseCsvRecordErrorKind_Read, IoError, Path)
);
// clang-format on

typedef struct {
    ParseCsvRecordErrorKind kind;
    size_t i;
} ParseCsvRecordError;

// clang-format off
datatype(
    ParseCsvError,
    (ParseCsvE_OpenFile, IoError, Path),
    (ParseCsvE_Record, ParseCsvRecordError)
);
// clang-format on

Result(ParseCsvRecord, Unit, ParseCsvRecordError);
Result(ParseCsv, Unit, ParseCsvError);

static void ParseCsvRecordError_print(ParseCsvRecordError e, FILE *stream) {
    match(e.kind) {
        of(ParseCsvRecordErrorKind_Read, _, path) fprintf(stream, "Cannot read from %s", *path);
    }
}

static void ParseCsvError_print(ParseCsvError e, FILE *stream) {
    match(e) {
        of(ParseCsvE_OpenFile, _, path) fprintf(stream, "Cannot open file %s", *path);
        of(ParseCsvE_Record, record_e) fprintf(stream, "Cannot parse record #%zd", record_e->i);
    }
}

static ParseCsvResult parse_csv(Path path, Callback cb);
static ParseCsvRecordResult parse_csv_record(Path path, FILE *stream, size_t i, Callback cb);
static ParseCsvRecordResult
parse_csv_value(Path path, FILE *stream, size_t *i, size_t j, Callback cb);

static ParseCsvResult parse_csv(Path path, Callback cb) {
    ParseCsvResult result = ParseCsvOk(unit);

    FILE *stream = fopen(path, "r");
    if (!stream) {
        return Err(ParseCsv, OpenFile(errno, path));
    }

    for (size_t i = 0; !feof(stream); i++) {
        ParseCsvRecordResult r = parse_csv_record(path, stream, i, cb);
        ifLet(r, ParseCsvRecordE, e) {
            result = Err(ParseCsv, Record(*e));
            goto cleanup;
        }
    }

cleanup:
    fclose(stream);
    return result;
}

static ParseCsvRecordResult parse_csv_record(Path path, FILE *stream, size_t i, Callback cb) {
    for (size_t j = 0; !feof(stream); j++) {
        ParseCsvRecordResult r = parse_csv_value(path, stream, &i, j, cb);
        tryResult(r, ParseCsvRecord, _, {});
    }

    return ParseCsvRecordOk(unit);
}

static ParseCsvRecordResult
parse_csv_value(Path path, FILE *stream, size_t *i, size_t j, Callback cb) {
    char value[32] = {0};

    int c = fgetc(stream);
    while (!feof(stream)) {
        if (ferror(stream)) {
            return ParseCsvRecordE(
                (ParseCsvRecordError){ParseCsvRecordErrorKind_Read(errno, path), *i});
        }

        switch (c) {
        case ',':
            cb(*i, j, value);
            return ParseCsvRecordOk(unit);
        case '\n':
            (*i)++;
            return ParseCsvRecordOk(unit);
        default:
            if (!isblank(c)) {
                strncat(value, (char[]){c, '\0'}, sizeof value);
            }
        }

        c = fgetc(stream);
    }
}

static void cb(size_t i, size_t j, const char *value) {
    printf("[%zd][%zd]: %s\n", i, j, value);
}

int main(void) {
    ParseCsvResult result = parse_csv("data/table.csv", cb);

    ifLet(result, ParseCsvE, e) {
        fprintf(stderr, "Failed to parse CSV: ");
        ParseCsvError_print(*e, stderr);
        fprintf(stderr, "\n");
    }
}
