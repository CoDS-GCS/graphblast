#ifndef GRB_BACKEND_APSPIE_DESCRIPTOR_HPP
#define GRB_BACKEND_APSPIE_DESCRIPTOR_HPP

#include <vector>

#include <moderngpu.cuh>

#include "graphblas/backend/apspie/util.hpp"

namespace graphblas
{
namespace backend
{
  class Descriptor
  {
    public:
    Descriptor() : desc_{ GrB_DEFAULT, GrB_DEFAULT, GrB_DEFAULT, GrB_DEFAULT, 
                          GrB_FIXEDROW, GrB_32, GrB_32, GrB_128, GrB_PUSHPULL,
                          GrB_APSPIELB, GrB_16 },
                   h_buffer_(NULL), h_size_(0), d_buffer_(NULL), d_size_(0),
                   d_context_(mgpu::CreateCudaDevice(0)) {}

    // Default Destructor
    ~Descriptor();

    // C API Methods
    Info set( Desc_field field, Desc_value  value );
    Info get( Desc_field field, Desc_value* value ) const;

    // Private methods
    Info toggleTranspose( Desc_field field );

    private:
    Info resize( size_t target );

    private:
    Desc_value desc_[GrB_NDESCFIELD];
    void*      h_buffer_;
    size_t     h_size_;
    void*      d_buffer_;
    size_t     d_size_;

    // MGPU context
    mgpu::ContextPtr d_context_;
  };

  Descriptor::~Descriptor()
  {
    if( h_buffer_!=NULL ) free(h_buffer_);
    if( d_buffer_!=NULL ) CUDA( cudaFree(d_buffer_) );
  }

  Info Descriptor::set( Desc_field field, Desc_value value )
  {
    desc_[field] = value;
    return GrB_SUCCESS;
  }

  Info Descriptor::get( Desc_field field, Desc_value* value ) const
  {
    *value = desc_[field];
    return GrB_SUCCESS;
  }

  // Toggles transpose vs. non-transpose
  Info Descriptor::toggleTranspose( Desc_field field )
  {
    if( desc_[field]!=GrB_DEFAULT ) desc_[field] = GrB_DEFAULT;
    else desc_[field] = GrB_TRAN;
    return GrB_SUCCESS;
  }

  Info Descriptor::resize( size_t target )
  {
    void* d_temp_buffer = d_buffer_;

    if( target>d_size_ )
    {
      CUDA( cudaMalloc(&d_buffer_, target*4) );
      if( d_temp_buffer!=NULL )
        CUDA( cudaMemcpy(d_buffer_, d_temp_buffer, d_size_*4, 
            cudaMemcpyDeviceToDevice) );
      d_size_ = target;

      CUDA( cudaFree( d_temp_buffer ) );
    }
    return GrB_SUCCESS;
  }

}  // backend
}  // graphblas

#endif  // GRB_DESCRIPTOR_HPP
