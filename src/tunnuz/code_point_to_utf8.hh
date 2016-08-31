#ifndef JSON_HELPER_CODE_POINT_TO_UTF_8_HH
#define JSON_HELPER_CODE_POINT_TO_UTF_8_HH

#include <stdexcept>
#include <string>

namespace JSON {

    namespace HELPER {

        /**
         * @brief Convert 16 bit code point to UTF-8 string.
         *
         * Conversion implemented according to:
         *
         * http://www.unicode.org/versions/Unicode6.2.0/ch03.pdf#G7404
         * Table 3-6. UTF-8 Bit Distribution
         * -------------------|------------|------------|------------|
         * Scalar Value       |  1st Byte  |  2nd Byte  |  3rd Byte  |
         * -------------------|------------|------------|------------|
         * 00000000 0xxxxxxx  |  0xxxxxxx  |            |            |  (1)
         * 00000yyy yyxxxxxx  |  110yyyyy  |  10xxxxxx  |            |  (2)
         * zzzzyyyy yyxxxxxx  |  1110zzzz  |  10yyyyyy  |  10xxxxxx  |  (3)
         * -------------------|------------|------------|------------|
         * 87654321 87654321  |  87654321  |  87654321  |  87654321  |
         * -------------------|------------|------------|------------|
         *
         * @param code_point
         *
         * @return string containing at most 3 bytes representing the code point
         */
        inline std::string code_point_to_utf8(const char16_t code_point)
        {
            /*  (1)  */
            if (code_point <= 0x007F) return { static_cast<char>(code_point) };

            /*  (2)  */
            if (code_point <= 0x07FF) {
                const unsigned char first_byte = 0xC0 + ((code_point & 0x07C0 ) >> 6);
                const unsigned char second_byte = 0x80 + (code_point & 0x003F);
                return { static_cast<char>(first_byte), static_cast<char>(second_byte) };
            }

            /*  (3)  */
            /* code_point <= 0xFFFF */
            const unsigned char first_byte = 0xE0 + ((code_point & 0xF000 ) >> 12);
            const unsigned char second_byte = 0x80 + ((code_point & 0x0FC0 ) >> 6);
            const unsigned char third_byte = 0x80 + (code_point & 0x003F);
            return {
                    static_cast<char>(first_byte),
                    static_cast<char>(second_byte),
                    static_cast<char>(third_byte) };
            

            return { };  // can never happen
        }

    }

}

#endif
