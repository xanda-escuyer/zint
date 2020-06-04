/*
    libzint - the open source barcode library
    Copyright (C) 2019 - 2020 Robin Stuart <rstuart114@gmail.com>

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the project nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
 */
/* vim: set ts=4 sw=4 et : */

#include "testcommon.h"

static void test_upce_length(int index, int debug) {

    testStart("");

    int ret;
    struct item {
        int symbology;
        unsigned char *data;
        int ret;
    };
    // s/\/\*[ 0-9]*\*\//\=printf("\/*%2d*\/", line(".") - line("'<"))
    struct item data[] = {
        /* 0*/ { BARCODE_UPCE, "12345", 0 },
        /* 1*/ { BARCODE_UPCE_CHK, "12345", ZINT_ERROR_INVALID_CHECK },
        /* 2*/ { BARCODE_UPCE_CHK, "12344", 0 }, // 4 is correct check digit
        /* 3*/ { BARCODE_UPCE, "123456", 0 },
        /* 4*/ { BARCODE_UPCE_CHK, "123456", ZINT_ERROR_INVALID_CHECK },
        /* 5*/ { BARCODE_UPCE_CHK, "123457", 0 }, // 7 is correct check digit
        /* 6*/ { BARCODE_UPCE, "1234567", 0 },
        /* 7*/ { BARCODE_UPCE_CHK, "1234567", ZINT_ERROR_INVALID_CHECK },
        /* 8*/ { BARCODE_UPCE_CHK, "1234565", 0 }, // 5 is correct check digit
        /* 9*/ { BARCODE_UPCE, "12345678", ZINT_ERROR_TOO_LONG },
        /*10*/ { BARCODE_UPCE_CHK, "12345678", ZINT_ERROR_INVALID_CHECK },
        /*11*/ { BARCODE_UPCE_CHK, "12345670", 0 }, // 0 is correct check digit
        /*12*/ { BARCODE_UPCE, "123456789", ZINT_ERROR_TOO_LONG },
        /*13*/ { BARCODE_UPCE_CHK, "123456789", ZINT_ERROR_TOO_LONG },
        /*14*/ { BARCODE_UPCE, "123406", ZINT_ERROR_INVALID_DATA }, // If last digit (emode) 6, 2nd last can't be zero
    };
    int data_size = sizeof(data) / sizeof(struct item);

    for (int i = 0; i < data_size; i++) {

        if (index != -1 && i != index) continue;

        struct zint_symbol *symbol = ZBarcode_Create();
        assert_nonnull(symbol, "Symbol not created\n");

        symbol->symbology = data[i].symbology;
        symbol->debug |= debug;

        int length = strlen(data[i].data);

        ret = ZBarcode_Encode(symbol, data[i].data, length);
        assert_equal(ret, data[i].ret, "i:%d ret %d != %d\n", i, ret, data[i].ret);

        ZBarcode_Delete(symbol);
    }

    testFinish();
}

// Note requires ZINT_SANITIZE to be set
static void test_upca_print(int index, int debug) {

    testStart("");

    int ret;
    struct item {
        int symbology;
        unsigned char *data;
        int ret;
    };
    // s/\/\*[ 0-9]*\*\//\=printf("\/*%3d*\/", line(".") - line("'<"))
    struct item data[] = {
        /*  0*/ { BARCODE_UPCA, "01234567890", 0 },
    };
    int data_size = sizeof(data) / sizeof(struct item);

    for (int i = 0; i < data_size; i++) {

        if (index != -1 && i != index) continue;

        struct zint_symbol *symbol = ZBarcode_Create();
        assert_nonnull(symbol, "Symbol not created\n");

        symbol->symbology = data[i].symbology;
        symbol->debug |= debug;

        int length = strlen(data[i].data);

        ret = ZBarcode_Encode(symbol, data[i].data, length);
        assert_equal(ret, data[i].ret, "i:%d ret %d != %d\n", i, ret, data[i].ret);

        strcpy(symbol->outfile, "out.gif");
        ret = ZBarcode_Print(symbol, 0);
        assert_zero(ret, "i:%d %s ZBarcode_Print %s ret %d != 0\n", i, testUtilBarcodeName(data[i].symbology), symbol->outfile, ret);

        assert_zero(remove(symbol->outfile), "i:%d remove(%s) != 0\n", i, symbol->outfile);

        ZBarcode_Delete(symbol);
    }

    testFinish();
}

static void test_isbn(int index, int debug) {

    testStart("");

    int ret;
    struct item {
        unsigned char *data;
        int ret_encode;
        int ret_vector;
    };
    // s/\/\*[ 0-9]*\*\//\=printf("\/*%2d*\/", line(".") - line("'<"))
    struct item data[] = {
        /* 0*/ { "0", 0, 0 }, // Left zero-padded if < 10 chars
        /* 1*/ { "12345678", ZINT_ERROR_INVALID_CHECK, -1 },
        /* 2*/ { "12345679", 0, 0 }, // 9 is correct check digit
        /* 3*/ { "123456789", 0, 0 },
        /* 4*/ { "0123456789", 0, 0 },
        /* 5*/ { "1234567890", ZINT_ERROR_INVALID_CHECK, -1 },
        /* 6*/ { "123456789X", 0, 0 }, // X is correct check digit
        /* 7*/ { "8175257660", 0, 0 }, // 0 is correct check digit
        /* 8*/ { "0590764845", 0, 0 }, // 5 is correct check digit
        /* 9*/ { "0906495741", 0, 0 }, // 1 is correct check digit
        /*10*/ { "0140430016", 0, 0 }, // 6 is correct check digit
        /*11*/ { "0571086187", 0, 0 }, // 7 is correct check digit
        /*12*/ { "0486600882", 0, 0 }, // 2 is correct check digit
        /*13*/ { "12345678901", ZINT_ERROR_TOO_LONG, -1 },
        /*14*/ { "123456789012", ZINT_ERROR_TOO_LONG, -1 },
        /*15*/ { "1234567890123", ZINT_ERROR_INVALID_DATA, -1 },
        /*16*/ { "9784567890120", 0, 0 }, // 0 is correct check digit
        /*17*/ { "9783161484100", 0, 0 }, // 0 is correct check digit
        /*18*/ { "9781846688225", 0, 0 }, // 5 is correct check digit
        /*19*/ { "9781847657954", 0, 0 }, // 4 is correct check digit
        /*20*/ { "9781846688188", 0, 0 }, // 8 is correct check digit
        /*21*/ { "9781847659293", 0, 0 }, // 3 is correct check digit
        /*22*/ { "97845678901201", ZINT_ERROR_TOO_LONG, -1 },
        /*23*/ { "3954994+12", 0, 0 },
        /*24*/ { "3954994+12345", 0, 0 },
        /*25*/ { "3954994+123456", ZINT_ERROR_TOO_LONG, -1 },
        /*26*/ { "3954994+", 0, 0 },
        /*27*/ { "61954993+1", 0, 0 },
        /*28*/ { "61954993+123", 0, 0 },
        /*29*/ { "361954999+12", 0, 0 },
        /*30*/ { "361954999+1234", 0, 0 },
        /*31*/ { "361954999+12", 0, 0 },
        /*32*/ { "199900003X+12", 0, 0 },
        /*33*/ { "199900003X+12345", 0, 0 },
        /*34*/ { "9791234567896+12", 0, 0 },
        /*35*/ { "9791234567896+12345", 0, 0 },
        /*36*/ { "9791234567896+", 0, 0 },
        /*37*/ { "97912345678961+", ZINT_ERROR_TOO_LONG, -1 },
        /*38*/ { "97912345678961+12345", ZINT_ERROR_TOO_LONG, -1 },
        /*39*/ { "9791234567896+123456", ZINT_ERROR_TOO_LONG, -1 },
    };
    int data_size = sizeof(data) / sizeof(struct item);

    for (int i = 0; i < data_size; i++) {

        if (index != -1 && i != index) continue;

        struct zint_symbol *symbol = ZBarcode_Create();
        assert_nonnull(symbol, "Symbol not created\n");

        symbol->symbology = BARCODE_ISBNX;
        symbol->debug |= debug;

        int length = strlen(data[i].data);

        ret = ZBarcode_Encode(symbol, data[i].data, length);
        assert_equal(ret, data[i].ret_encode, "i:%d ZBarcode_Encode ret %d != %d, errtxt %s\n", i, ret, data[i].ret_encode, symbol->errtxt);

        if (data[i].ret_vector != -1) {
            ret = ZBarcode_Buffer_Vector(symbol, 0);
            assert_equal(ret, data[i].ret_vector, "i:%d ZBarcode_Buffer_Vector ret %d != %d\n", i, ret, data[i].ret_vector);
        }

        ZBarcode_Delete(symbol);
    }

    testFinish();
}

static void test_vector_same(int index, int debug) {

    testStart("");

    int ret;
    struct item {
        int symbology;
        unsigned char *data;
        int ret_encode;
        int ret_vector;
    };
    struct item data[] = {
        /* 0*/ { BARCODE_UPCE, "123456", 0, 0 },
        /* 1*/ { BARCODE_UPCE_CHK, "1234565", 0, 0 }, // 5 is correct check digit
        /* 2*/ { BARCODE_ISBNX, "0195049969", 0, 0 }, // 9 is correct check digit
    };
    int data_size = sizeof(data) / sizeof(struct item);

    for (int i = 0; i < data_size; i++) {

        if (index != -1 && i != index) continue;

        struct zint_vector *vectors[4];
        int vectors_size = sizeof(vectors) / sizeof(struct zint_vector*);

        for (int j = 0; j < vectors_size; j++) {
            struct zint_symbol *symbol = ZBarcode_Create();
            assert_nonnull(symbol, "Symbol not created\n");

            symbol->symbology = data[i].symbology;
            symbol->debug |= debug;

            int length = strlen(data[i].data);

            ret = ZBarcode_Encode(symbol, data[i].data, length);
            assert_equal(ret, data[i].ret_encode, "i:%d ZBarcode_Encode ret %d != %d\n", i, ret, data[i].ret_encode);

            ret = ZBarcode_Buffer_Vector(symbol, 0);
            assert_equal(ret, data[i].ret_vector, "i:%d ZBarcode_Buffer_Vector ret %d != %d\n", i, ret, data[i].ret_vector);

            assert_nonnull(symbol->vector, "i:%d symbol->vector NULL\n", i);
            vectors[j] = testUtilVectorCpy(symbol->vector);

            ZBarcode_Delete(symbol);
        }

        for (int j = 1; j < vectors_size; j++) {
            ret = testUtilVectorCmp(vectors[j - 1], vectors[j]);
            assert_zero(ret, "i:%d testUtilVectorCmp ret %d != 0\n", i, ret);
        }

        for (int j = 0; j < vectors_size; j++) {
            struct zint_symbol symbol_vector;
            symbol_vector.vector = vectors[j];
            vector_free(&symbol_vector);
        }
    }

    testFinish();
}

static void test_encode(int index, int generate, int debug) {

    testStart("");

    int ret;
    struct item {
        int symbology;
        unsigned char *data;
        int ret;

        int expected_rows;
        int expected_width;
        char *comment;
        char *expected;
    };
    struct item data[] = {
        /*  0*/ { BARCODE_UPCA, "1234567890", 0, 1, 95, "GS1 General Specifications 20.0 Figure 5.1-1 left",
                    "10100011010011001001001101111010100011011000101010101000010001001001000111010011100101001110101"
                },
        /*  1*/ { BARCODE_EANX, "4512345678906", 0, 1, 95, "GS1 General Specifications 20.0 Figure 5.1-1 right",
                    "10101100010110011001001101111010011101011100101010101000010001001001000111010011100101010000101"
                },
    };
    int data_size = ARRAY_SIZE(data);

    for (int i = 0; i < data_size; i++) {

        if (index != -1 && i != index) continue;

        struct zint_symbol *symbol = ZBarcode_Create();
        assert_nonnull(symbol, "Symbol not created\n");

        int length = testUtilSetSymbol(symbol, data[i].symbology, -1 /*input_mode*/, -1 /*eci*/, -1 /*option_1*/, -1, -1, -1 /*output_options*/, data[i].data, -1, debug);

        ret = ZBarcode_Encode(symbol, data[i].data, length);
        assert_equal(ret, data[i].ret, "i:%d ZBarcode_Encode ret %d != %d\n", i, ret, data[i].ret);

        if (generate) {
            printf("        /*%3d*/ { %s, \"%s\", %s, %d, %d, \"%s\",\n",
                                i, testUtilBarcodeName(data[i].symbology), data[i].data, testUtilErrorName(data[i].ret), symbol->rows, symbol->width, data[i].comment);
            testUtilModulesDump(symbol, "                    ", "\n");
            printf("                },\n");
        } else {
            if (ret < 5) {
                assert_equal(symbol->rows, data[i].expected_rows, "i:%d symbol->rows %d != %d (%s)\n", i, symbol->rows, data[i].expected_rows, data[i].data);
                assert_equal(symbol->width, data[i].expected_width, "i:%d symbol->width %d != %d (%s)\n", i, symbol->width, data[i].expected_width, data[i].data);

                if (ret == 0) {
                    int width, row;
                    ret = testUtilModulesCmp(symbol, data[i].expected, &width, &row);
                    assert_zero(ret, "i:%d testUtilModulesCmp ret %d != 0 width %d row %d (%s)\n", i, ret, width, row, data[i].data);
                }
            }
        }

        ZBarcode_Delete(symbol);
    }

    testFinish();
}

// #181 Christian Hartlage OSS-Fuzz
static void test_fuzz(int index, int debug) {

    testStart("");

    int ret;
    struct item {
        int symbology;
        unsigned char *data;
        int length;
        int ret;
    };
    // s/\/\*[ 0-9]*\*\//\=printf("\/*%2d*\/", line(".") - line("'<"))
    struct item data[] = {
        /* 0*/ { BARCODE_EANX, "55++15", -1, ZINT_ERROR_INVALID_DATA },
        /* 1*/ { BARCODE_EANX, "+123456789012345678", -1, ZINT_ERROR_TOO_LONG },
        /* 2*/ { BARCODE_EANX_CHK, "+123456789012345678", -1, ZINT_ERROR_TOO_LONG },
        /* 3*/ { BARCODE_UPCA, "+123456789012345678", -1, ZINT_ERROR_TOO_LONG },
        /* 4*/ { BARCODE_UPCA_CHK, "+123456789012345678", -1, ZINT_ERROR_TOO_LONG },
        /* 5*/ { BARCODE_UPCE, "+123456789012345678", -1, ZINT_ERROR_TOO_LONG },
        /* 6*/ { BARCODE_UPCE_CHK, "+123456789012345678", -1, ZINT_ERROR_TOO_LONG },
        /* 7*/ { BARCODE_ISBNX, "+123456789012345678", -1, ZINT_ERROR_TOO_LONG },
        /* 8*/ { BARCODE_EANX, "+12345", -1, 0 },
        /* 9*/ { BARCODE_EANX, "+123456", -1, ZINT_ERROR_TOO_LONG },
    };
    int data_size = sizeof(data) / sizeof(struct item);

    for (int i = 0; i < data_size; i++) {

        if (index != -1 && i != index) continue;

        struct zint_symbol *symbol = ZBarcode_Create();
        assert_nonnull(symbol, "Symbol not created\n");

        symbol->symbology = data[i].symbology;
        symbol->debug |= debug;

        int length = data[i].length;
        if (length == -1) {
            length = strlen(data[i].data);
        }

        ret = ZBarcode_Encode(symbol, data[i].data, length);
        assert_equal(ret, data[i].ret, "i:%d ret %d != %d (%s)\n", i, ret, data[i].ret, symbol->errtxt);

        ZBarcode_Delete(symbol);
    }

    testFinish();
}

int main(int argc, char *argv[]) {

    testFunction funcs[] = { /* name, func, has_index, has_generate, has_debug */
        { "test_upce_length", test_upce_length, 1, 0, 1 },
        { "test_upca_print", test_upca_print, 1, 0, 1 },
        { "test_isbn", test_isbn, 1, 0, 1 },
        { "test_vector_same", test_vector_same, 1, 0, 1 },
        { "test_encode", test_encode, 1, 1, 1 },
        { "test_fuzz", test_fuzz, 1, 0, 1 },
    };

    testRun(argc, argv, funcs, ARRAY_SIZE(funcs));

    testReport();

    return 0;
}
