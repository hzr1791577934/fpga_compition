#include "top.hpp"

void dut(unsigned numVert,
		unsigned numEdge,
		unsigned* offset,
		unsigned* column,
		unsigned* color,
		unsigned* tmp0,
		unsigned* tmp1,
		unsigned* tmp2,
		unsigned* tmp3) {
// clang-format off

#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 1 num_read_outstanding = \
    16 max_write_burst_length = 2 max_read_burst_length = 256 bundle = gmem0 port = offset

#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 1 num_read_outstanding = \
    16 max_write_burst_length = 2 max_read_burst_length = 256 bundle = gmem1 port = column

#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 1 num_read_outstanding = \
    16 max_write_burst_length = 2 max_read_burst_length = 256 bundle = gmem2 port = color

#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 1 num_read_outstanding = \
    16 max_write_burst_length = 2 max_read_burst_length = 256 bundle = gmem3 port = tmp0

#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 1 num_read_outstanding = \
    16 max_write_burst_length = 2 max_read_burst_length = 256 bundle = gmem4 port = tmp1

#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 1 num_read_outstanding = \
    16 max_write_burst_length = 2 max_read_burst_length = 256 bundle = gmem5 port = tmp2

#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 1 num_read_outstanding = \
    16 max_write_burst_length = 2 max_read_burst_length = 256 bundle = gmem6 port = tmp3


    unsigned numColor=0;
    static unsigned offset_bram[600000];
#pragma HLS ARRAY_PARTITION variable=offset_bram dim=1 factor=2 cyclic
    initial:
	for(int i=0;i<600000;i++){
		offset_bram[i] = offset[i];
		if(i==numVert) break;
        color[i] = 0;
		tmp1[i]=offset[i+1]-offset_bram[i];
	}

	//将顶点按照度数排序
	//tmp3存储排序后的度数，tmp2存储按照度数排序后的顶点位置
	sort_outer_loop:
	for(int i=0;i<20000;i++){
		if(i==numVert) break;
		unsigned int item=tmp1[i];
		unsigned int vert=i;
		sort_inner_loop:
		for(int j=numVert-1;j>=0;j--){
			unsigned int t;
			unsigned int v;
			if(j>i){
				t=tmp3[j];
				v=tmp2[j];
			}else if(j>0&&tmp3[j-1]<item){
				t=tmp3[j-1];
				v=tmp2[j-1];
			}else{
				t=item;
				v=vert;
				if(j>0){
					item=tmp3[j-1];
					vert=tmp2[j-1];
				}

			}
			tmp3[j]=t;
			tmp2[j]=v;
		}
	}

	unsigned int colored_vertices_cnt=0,colorNum=0;
	color_outer_loop:
	while(colored_vertices_cnt<numVert){
		colorNum++;
		color_middle_loop:
		for(int k=0;k<numVert;k++){
			int i;
			if(k<20000) i=tmp2[k];
			else i=k;
			if(color[i]==0){
				unsigned ok=1;
				unsigned start=offset_bram[i];
				unsigned end =offset_bram[i+1];
				color_inner_loop:
				for(int j=start;j<end;j++){
					if(color[column[j]]==colorNum){
						ok=0;
						break;
					}
				}
				if(ok){
					color[i]=colorNum;
					colored_vertices_cnt++;
				}

			}
		}
	}
}


