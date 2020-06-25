#include "DocumentExecutor.h"

template<typename TPath, typename TFile>
DocumentExecutor<TPath, TFile>::DocumentExecutor(TPath& path, TFile& file):
    _path(path),
    _file(file)
{

}

template<typename TPath, typename TFile>
Utf8String
DocumentExecutor<TPath, TFile>::execute(const TPath& file) const
{

}