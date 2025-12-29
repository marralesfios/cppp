#pragma once
#include<filesystem>
#include<stdexcept>
#include<fstream>
#include<cstdint>
#include<cstring>
#include<string>
#include<memory>
#include<limits>
#include<bit>
#include"binary.hpp"
namespace cppp{
    struct file_error : std::exception{};
    struct operation_failed : file_error{};
    class BinaryFile{
        using traits = std::char_traits<char>;
        mutable std::fstream fs;
        public:
            //Make tellg() return value opaque, as it should be.
            class pos_buf{
                friend class BinaryFile;
                std::fstream::pos_type v;
                pos_buf(std::fstream::pos_type v) : v(v){}
            };
            BinaryFile(){
                fs.exceptions(std::ios_base::badbit);
            }
            BinaryFile(std::filesystem::path path,std::ios::openmode mode) : fs(path,mode){
                fs.exceptions(std::ios_base::badbit);
                errchk();
            }
            void open(std::filesystem::path path,std::ios::openmode mode){
                fs.open(path,mode);
                errchk();
            }
            pos_buf tell() const{
                return fs.tellg();
            }
            void seek(pos_buf pos){
                fs.rdbuf()->pubseekpos(pos.v);
            }
            void seek(std::int64_t pos,std::ios::seekdir where=std::ios::beg){
                fs.rdbuf()->pubseekoff(pos,where);
            }
            std::size_t read(buffer buf){
                fs.read(reinterpret_cast<char*>(buf.data()),buf.size());
                if(fs.eof()){
                    fs.clear(fs.rdstate()&~fs.failbit);
                }
                errchk();
                return fs.gcount();
            }
            void errchk() const{
                if(fs.fail()){
                    throw operation_failed();
                }
            }
            void errclr(){
                fs.clear();
            }
            bool fread(buffer buf){
                return read(buf)==buf.size();
            }
            bool safe_read(buffer buf){
                std::unique_ptr<std::byte[]> dup{new std::byte[buf.size()]};
                bool state;
                if((state=fread({dup.get(),buf.size()}))){
                    std::memcpy(buf.data(),dup.get(),buf.size());
                }
                return state;
            }
            template<std::size_t l>
            bool safe_read(staticbuffer<l> buf){
                std::array<std::byte,l> dup;
                bool state;
                if((state=fread(dup))){
                    std::memcpy(buf.data(),dup.data(),l);
                }
                return state;
            }
            bool eof() const{
                return fs.eof();
            }
            std::byte readbr(){
                return std::bit_cast<std::byte>(traits::to_char_type(fs.get()));
            }
            std::uint8_t readb(){
                return std::bit_cast<std::uint8_t>(traits::to_char_type(fs.get()));
            }
            template<typename I>
            I readl(){
                std::array<std::byte,sizeof(I)> buf;
                if(!fread(buf)){
                    throw std::runtime_error("BinaryFile::readl(): not enough data");
                }
                return read<I>(buf.data());
            }
            void write(std::string_view s){
                fs.write(s.data(),s.size());
            }
            void write(std::u8string_view s){
                fs.write(reinterpret_cast<const char*>(s.data()),s.size());
            }
            void write(frozenbuffer buf){
                fs.write(reinterpret_cast<const char*>(buf.data()),buf.size());
            }
            void writeb(std::byte b){
                fs.put(std::bit_cast<char>(b));
            }
            void writeb(std::uint8_t v){
                fs.put(std::bit_cast<char>(v));
            }
            template<typename I>
            void writel(I v){
                std::array<std::byte,sizeof(I)> buf;
                write<I>(buf.data(),v);
                write(buf);
            }
            void flush(){
                fs.flush();
            }
            void close(){
                fs.close();
            }
    };
}
