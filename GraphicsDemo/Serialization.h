#ifndef SERIALIZATION_H_
#define SERIALIZATION_H_

__declspec(align(32))
struct RecordHeader
{
    enum RecordType : int16_t
    {
        RT_INVALID = 0,
        
        RT_STL_VECTOR,
        RT_OBJECT,
        RT_MATERIAL,
        RT_GL_TEXTURE_MAP,
        RT_MESH,
        RT_ATTRIBUTE_ARRAY,
        RT_GL_ATTRIBUTE_ARRAY,
    };

    RecordHeader()
        : type(RecordType::RT_INVALID)
        , szData(0)
    {
    }

    RecordType type;
    uint32_t szData;
};

class Serialization
{
public:
    template <typename T>
    static void Write(std::ostream& os, const T& t)
    {
        os.write(reinterpret_cast<const char*>(&t), sizeof(T));
    }

    template<>
    static void Write(std::ostream& os, const std::string& t)
    {
        int size = static_cast<int>(t.size());
        Write(os, size);
        os.write(t.data(), size);
    }

    template <typename T>
    static void Read(std::istream& is, T& t)
    {
        is.read(reinterpret_cast<char*>(&t), sizeof(T));
    }

    template <>
    static void Read(std::istream& is, std::string& t)
    {
        int size;
        Read(is, size);
        t.reserve(size);
        // // ref : https://stackoverflow.com/questions/1816319/reading-directly-from-an-stdistream-into-an-stdstring
        // std::copy_n(std::istreambuf_iterator<char>(is), size, std::insert_iterator<std::string>(t, t.begin()));

        std::vector<char> buffer(size, '\0');
        is.read(buffer.data(), buffer.size());
        t.assign(buffer.begin(), buffer.end());
    }
};

#endif
