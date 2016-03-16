#ifndef STORAGE_GRAPHVIZ_H
#define STORAGE_GRAPHVIZ_H


namespace storage
{

    /**
     * Bitfield to control graphviz output.
     *
     * If TOOLTIP is enabled the output of the others fields is added to the
     * tooltip, otherwise they are added to the label. Not all flags apply to
     * all graphs or all objects (devices, holders and actions).
     */
    enum class GraphvizFlags : unsigned int
    {
	NONE = 0x0,
	TOOLTIP = 0x1,
	CLASSNAME = 0x2,
	SID = 0x4,
	SIZE = 0x8
    };


    GraphvizFlags operator|(GraphvizFlags a, GraphvizFlags b);
    GraphvizFlags operator&(GraphvizFlags a, GraphvizFlags b);

    bool operator&&(GraphvizFlags a, GraphvizFlags b);

}


#endif
