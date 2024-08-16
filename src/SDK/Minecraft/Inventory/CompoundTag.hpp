//
// Created by vastrakai on 7/5/2024.
//

#pragma once

#include <string>
#include <map>
#include <memory>

class Tag
{
public:
    enum class Type : uint8_t {
        End,
        Byte,
        Short,
        Int,
        Int64,
        Float,
        Double,
        ByteArray,
        String,
        List,
        Compound,
        IntArray,
    };

    virtual ~Tag();
    virtual void deleteChildren();
    virtual void Func2();
    virtual void load(class IDataInput&);
    virtual std::string toString();
    virtual Type getId();
    virtual bool equals(const Tag&) const;
    virtual void print(class PrintStream&) const;
    virtual void print(std::string const&, class PrintStream&) const;
    virtual std::unique_ptr<Tag> copy() const = 0;
    virtual void Func10();
};

class CompoundTagVariant
{
    PAD(0x28);
    Tag::Type type; //0x0028
    PAD(0x7);
public:
    inline Tag::Type getTagType() {
        return this->type;
    }
    class StringTag : public Tag
    {
    public:
        std::string val;
    };
    inline StringTag* asStringTag() {
        return (StringTag*)this;
    }
};


class CompoundTag {
public:
    std::map<std::string, CompoundTagVariant> data;

    virtual ~CompoundTag();
    virtual void deleteChildren(void);
    virtual void write(class IDataOutput&);
    virtual void load(class IDataInput&);
    virtual std::string toString(void);
    virtual void getId(void);
    virtual void equals(class Tag const&);
    virtual void print(class PrintStream&);
    virtual void print(std::string const&, class PrintStream&);
    virtual void copy(void);
    virtual void hash(void);
};
