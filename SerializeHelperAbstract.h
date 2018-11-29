//
// Created by quals on 2018/11/29.
//

#ifndef BPTDEMO_SERIALIZEHELPERABSTRACT_H
#define BPTDEMO_SERIALIZEHELPERABSTRACT_H


template<class T>
class SerializeHelperAbstract {
public:
    virtual bool Serialize(const T *obj, const char *filename) = 0;

    virtual bool Deserialize(T *obj, const char *filename) = 0;
};


#endif //BPTDEMO_SERIALIZEHELPERABSTRACT_H
