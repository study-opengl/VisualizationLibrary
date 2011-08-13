/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.org                                               */
/*                                                                                    */
/*  Copyright (c) 2005-2010, Michele Bosi                                             */
/*  All rights reserved.                                                              */
/*                                                                                    */
/*  Redistribution and use in source and binary forms, with or without modification,  */
/*  are permitted provided that the following conditions are met:                     */
/*                                                                                    */
/*  - Redistributions of source code must retain the above copyright notice, this     */
/*  list of conditions and the following disclaimer.                                  */
/*                                                                                    */
/*  - Redistributions in binary form must reproduce the above copyright notice, this  */
/*  list of conditions and the following disclaimer in the documentation and/or       */
/*  other materials provided with the distribution.                                   */
/*                                                                                    */
/*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND   */
/*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED     */
/*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE            */
/*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR  */
/*  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES    */
/*  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      */
/*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON    */
/*  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           */
/*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS     */
/*  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                      */
/*                                                                                    */
/**************************************************************************************/

// mic fixme
#include <vlCore/SRF.hpp>
#include "BaseDemo.hpp"
#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/DrawPixels.hpp>
#include <vlGraphics/Light.hpp>
#include <vlGraphics/Geometry.hpp>
#include <vlCore/BufferedStream.hpp>
#include <vlCore/DiskFile.hpp>
#include <vlGraphics/MultiDrawElements.hpp>
#include <vlGraphics/TriangleStripGenerator.hpp>

using namespace vl;

class App_VLSRF: public BaseDemo
{
public:
  void srfPrelink_Geometry(Geometry* geom)
  {
    // --- generate UIDs ---
    // geometry itself
    generateUID(geom, "geometry_");
    // vertex arrays
    generateUID(geom->vertexArray(), "vertex_array_");
    generateUID(geom->normalArray(), "normal_array");
    generateUID(geom->colorArray(),  "color_array");
    generateUID(geom->secondaryColorArray(), "secondary_color_array");
    generateUID(geom->fogCoordArray(), "fogcoord_array");
    for(size_t i=0; i<VL_MAX_TEXTURE_UNITS; ++i)
      generateUID(geom->texCoordArray(i), "texcoord_array_");
    for(size_t i=0; i<VL_MAX_GENERIC_VERTEX_ATTRIB; ++i)
      if (geom->vertexAttribArray(i))
        generateUID(geom->vertexAttribArray(i)->data(), "vertexattrib_array_");
    // draw elements
    for(int i=0; i<geom->drawCalls()->size(); ++i)
    {
      generateUID(geom->drawCalls()->at(i), "drawcall_");
      generateUID(geom->drawCalls()->at(i)->patchParameter(), "patchparam_");
    }
  }

  const char* getBoolCR(bool ok)
  {
    return ok ? "true\n" : "false\n";
  }

  void srfExport_Renderable(Renderable* ren)
  {
    if (mObject_Ref_Count[ren] > 1)
      mSRFString += indent() + "ID = " + getUID(ren) + "\n";
    mSRFString += indent() + "VBOEnabled = " + getBoolCR(ren->isVBOEnabled());
    mSRFString += indent() + "DisplayListEnabled = " + getBoolCR(ren->isDisplayListEnabled());
    mSRFString += indent() + "AABB = "; mAssign = true; srfExport_AABB(ren->boundingBox());
    mSRFString += indent() + "Sphere = "; mAssign = true; srfExport_Sphere(ren->boundingSphere());
  }

  void srfExport_AABB(const AABB& aabb)
  {
    // AABBs are always inlined
    mSRFString += indent() + "<AABB>\n";
    mSRFString += indent() + "{\n"; 
    ++mIndent;
    mSRFString += indent() + String::printf("Min = ( %f %f %f )\n", aabb.minCorner().x(), aabb.minCorner().y(), aabb.minCorner().z() );
    mSRFString += indent() + String::printf("Max = ( %f %f %f )\n", aabb.maxCorner().x(), aabb.maxCorner().y(), aabb.maxCorner().z() );
    --mIndent;
    mSRFString += indent() + "}\n"; 
  }

  void srfExport_Sphere(const Sphere& sphere)
  {
    // AABBs are always inlined
    mSRFString += indent() + "<Sphere>\n";
    mSRFString += indent() + "{\n"; 
    ++mIndent;
    mSRFString += indent() + String::printf("Center = ( %f %f %f )\n", sphere.center().x(), sphere.center().y(), sphere.center().z() );
    mSRFString += indent() + String::printf("Radius = %f\n", sphere.radius() );
    --mIndent;
    mSRFString += indent() + "}\n";
  }

  void srfExport_Geometry(Geometry* geom)
  {
    if (isDefined(geom))
    {
      mSRFString += indent() + getUID(geom) + "\n";
      return;
    }
    else
      mAlreadyDefined.insert(geom);

    mSRFString += indent() + "<Geometry>\n";
    mSRFString += indent() + "{\n"; 
    ++mIndent;
    srfExport_Renderable(geom);
    // vertex arrays
    if (geom->vertexArray()) { mSRFString += indent() + "VertexArray = ";  mAssign = true; srfExport_Array(geom->vertexArray()); }
    if (geom->normalArray()) { mSRFString += indent() + "NormalArray = ";  mAssign = true; srfExport_Array(geom->normalArray()); }
    if (geom->colorArray()) { mSRFString += indent() + "ColorArray = ";  mAssign = true; srfExport_Array(geom->colorArray()); }
    if (geom->secondaryColorArray()) { mSRFString += indent() + "SecondaryArray = ";  mAssign = true; srfExport_Array(geom->secondaryColorArray()); }
    if (geom->fogCoordArray()) { mSRFString += indent() + "FogCoordArray = ";  mAssign = true; srfExport_Array(geom->fogCoordArray()); }
    for( int i=0; i<VL_MAX_TEXTURE_UNITS; ++i)
      if (geom->texCoordArray(i)) { mSRFString += indent() + String::printf("TexCoordArray%d = ", i);  mAssign = true; srfExport_Array(geom->texCoordArray(i)); }
    for(size_t i=0; i<VL_MAX_GENERIC_VERTEX_ATTRIB; ++i)
      if (geom->vertexAttribArray(i))
      {
        if (geom->vertexAttribArray(i)) { mSRFString += indent() + String::printf("VertexAttribArray%d = ",i); mAssign = true; srfExport_VertexAttribInfo(geom->vertexAttribArray(i)); }
      }
      // draw calls
    for(int i=0; i<geom->drawCalls()->size(); ++i)
    {
      mSRFString += indent() + "DrawCall = ";  mAssign = true; srfExport_DrawCall(geom->drawCalls()->at(i));
    }
    --mIndent;
    mSRFString += indent() + "}\n";
  }

  void srfExport_VertexAttribInfo(VertexAttribInfo* info)
  {
    if (isDefined(info))
    {
      mSRFString += indent() + getUID(info) + "\n";
      return;
    }
    else
      mAlreadyDefined.insert(info);

      mSRFString += indent() + "<VertexAttribInfo>\n";
      mSRFString += indent() + "{\n";
      ++mIndent;
      if (mObject_Ref_Count[info] > 1)
        mSRFString += indent() + "ID = " + getUID(info) + "\n";
      // mSRFString += indent() + String::printf("AttribLocation = %d\n", info->attribLocation());
      mSRFString += indent() + "Data = "; mAssign = true; srfExport_Array(info->data());
      mSRFString += indent() + "Normalize = " + (info->normalize() ? "true\n" : "false\n");
      mSRFString += indent() + "Interpretation = ";
      switch(info->interpretation())
      {
      case VAI_NORMAL: mSRFString += "VAI_NORMAL\n"; break;
      case VAI_INTEGER: mSRFString += "VAI_INTEGER\n"; break;
      case VAI_DOUBLE: mSRFString += "VAI_DOUBLE\n"; break;
      }
      --mIndent;
      mSRFString += indent() + "}\n";
  }

  template<typename T_Array>
  void srfExport_Array1(ArrayAbstract* arr_abstract, const char* name,const char* format, int elems_per_line = 40)
  {
    T_Array* arr = arr_abstract->as<T_Array>();
    mSRFString += indent() + name + "\n";
    mSRFString += indent() + "{\n";
    if (mObject_Ref_Count[arr] > 1)
      mSRFString += indent() + "\tID = " + getUID(arr) + "\n";
    if (arr->size()) // allow empty arrays this way
    {
      mSRFString += indent() + "\tValue = (\n\t\t" + indent();
      for(size_t i=0; i<arr->size(); ++i)
      {
        mSRFString += String::printf(format, arr->at(i) );
        if (i && (i % elems_per_line == 0) && i != arr->size()-1)
          mSRFString += "\n\t\t" + indent();
      }
      mSRFString += "\n" + indent() + "\t)\n";
    }
    mSRFString += indent() + "}\n";
  }

  template<typename T_Array>
  void srfExport_Array2(ArrayAbstract* arr_abstract, const char* name,const char* format, int elems_per_line = 30)
  {
    T_Array* arr = arr_abstract->as<T_Array>();
    mSRFString += indent() + name + "\n";
    mSRFString += indent() + "{\n";
    if (mObject_Ref_Count[arr] > 1)
      mSRFString += indent() + "\tID = " + getUID(arr) + "\n";
    if (arr->size()) // allow empty arrays this way
    {
      mSRFString += indent() + "\tValue = (\n\t\t" + indent();
      for(size_t i=0; i<arr->size(); ++i)
      {
        mSRFString += String::printf(format, arr->at(i).x(), arr->at(i).y() );
        if (i && (i % elems_per_line == 0) && i != arr->size()-1)
          mSRFString += "\n\t\t" + indent();
      }
      mSRFString += "\n" + indent() + "\t)\n";
    }
    mSRFString += indent() + "}\n";
  }

  template<typename T_Array>
  void srfExport_Array3(ArrayAbstract* arr_abstract, const char* name,const char* format, int elems_per_line = 20)
  {
    T_Array* arr = arr_abstract->as<T_Array>();
    mSRFString += indent() + name + "\n";
    mSRFString += indent() + "{\n";
    if (mObject_Ref_Count[arr] > 1)
      mSRFString += indent() + "\tID = " + getUID(arr) + "\n";
    if (arr->size()) // allow empty arrays this way
    {
      mSRFString += indent() + "\tValue = (\n\t\t" + indent();
      for(size_t i=0; i<arr->size(); ++i)
      {
        mSRFString += String::printf(format, arr->at(i).x(), arr->at(i).y(), arr->at(i).z() );
        if (i && (i % elems_per_line == 0) && i != arr->size()-1)
          mSRFString += "\n\t\t" + indent();
      }
      mSRFString += "\n" + indent() + "\t)\n";
    }
    mSRFString += indent() + "}\n";
  }

  template<typename T_Array>
  void srfExport_Array4(ArrayAbstract* arr_abstract, const char* name,const char* format, int elems_per_line = 10)
  {
    T_Array* arr = arr_abstract->as<T_Array>();
    mSRFString += indent() + name + "\n";
    mSRFString += indent() + "{\n";
    if (mObject_Ref_Count[arr] > 1)
      mSRFString += indent() + "\tID = " + getUID(arr) + "\n";
    if (arr->size()) // allow empty arrays this way
    {
      mSRFString += indent() + "\tValue = (\n\t\t" + indent();
      for(size_t i=0; i<arr->size(); ++i)
      {
        mSRFString += String::printf(format, arr->at(i).x(), arr->at(i).y(), arr->at(i).z(), arr->at(i).w() );
        if (i && (i % elems_per_line == 0) && i != arr->size()-1)
          mSRFString += "\n\t\t" + indent();
      }
      mSRFString += "\n" + indent() + "\t)\n";
    }
    mSRFString += indent() + "}\n";
  }

  void srfExport_Array(ArrayAbstract* arr_abstract)
  {
    if (isDefined(arr_abstract))
    {
      mSRFString += indent() + getUID(arr_abstract) + "\n";
      return;
    }
    else
      mAlreadyDefined.insert(arr_abstract);

    if(arr_abstract->classType() == ArrayUInt1::Type())
      srfExport_Array1<ArrayUInt1>(arr_abstract, "<ArrayUInt1>", "%u "); // mic fixme: these can actually be user specified
    else
    if(arr_abstract->classType() == ArrayUInt2::Type())
      srfExport_Array2<ArrayUInt2>(arr_abstract, "<ArrayUInt2>", "%u %u ");
    else
    if(arr_abstract->classType() == ArrayUInt3::Type())
      srfExport_Array3<ArrayUInt3>(arr_abstract, "<ArrayUInt3>", "%u %u %u ");
    else
    if(arr_abstract->classType() == ArrayUInt4::Type())
      srfExport_Array4<ArrayUInt4>(arr_abstract, "<ArrayUInt4>", "%u %u %u %u ");
    else

    if(arr_abstract->classType() == ArrayInt1::Type())
      srfExport_Array1<ArrayInt1>(arr_abstract, "<ArrayInt1>", "%d ");
    else
    if(arr_abstract->classType() == ArrayInt2::Type())
      srfExport_Array2<ArrayInt2>(arr_abstract, "<ArrayInt2>", "%d %d ");
    else
    if(arr_abstract->classType() == ArrayInt3::Type())
      srfExport_Array3<ArrayInt3>(arr_abstract, "<ArrayInt3>", "%d %d %d ");
    else
    if(arr_abstract->classType() == ArrayInt4::Type())
      srfExport_Array4<ArrayInt4>(arr_abstract, "<ArrayInt4>", "%d %d %d %d ");
    else

    if(arr_abstract->classType() == ArrayUShort1::Type())
      srfExport_Array1<ArrayUShort1>(arr_abstract, "<ArrayUShort1>", "%u ");
    else
    if(arr_abstract->classType() == ArrayUShort2::Type())
      srfExport_Array2<ArrayUShort2>(arr_abstract, "<ArrayUShort2>", "%u %u ");
    else
    if(arr_abstract->classType() == ArrayUShort3::Type())
      srfExport_Array3<ArrayUShort3>(arr_abstract, "<ArrayUShort3>", "%u %u %u ");
    else
    if(arr_abstract->classType() == ArrayUShort4::Type())
      srfExport_Array4<ArrayUShort4>(arr_abstract, "<ArrayUShort4>", "%u %u %u %u ");
    else

    if(arr_abstract->classType() == ArrayUShort1::Type())
      srfExport_Array1<ArrayUShort1>(arr_abstract, "<ArrayUShort1>", "%u ");
    else
    if(arr_abstract->classType() == ArrayUShort2::Type())
      srfExport_Array2<ArrayUShort2>(arr_abstract, "<ArrayUShort2>", "%u %u ");
    else
    if(arr_abstract->classType() == ArrayUShort3::Type())
      srfExport_Array3<ArrayUShort3>(arr_abstract, "<ArrayUShort3>", "%u %u %u ");
    else
    if(arr_abstract->classType() == ArrayUShort4::Type())
      srfExport_Array4<ArrayUShort4>(arr_abstract, "<ArrayUShort4>", "%u %u %u %u ");
    else

    if(arr_abstract->classType() == ArrayShort1::Type())
      srfExport_Array1<ArrayShort1>(arr_abstract, "<ArrayShort1>", "%d ");
    else
    if(arr_abstract->classType() == ArrayShort2::Type())
      srfExport_Array2<ArrayShort2>(arr_abstract, "<ArrayShort2>", "%d %d ");
    else
    if(arr_abstract->classType() == ArrayShort3::Type())
      srfExport_Array3<ArrayShort3>(arr_abstract, "<ArrayShort3>", "%d %d %d ");
    else
    if(arr_abstract->classType() == ArrayShort4::Type())
      srfExport_Array4<ArrayShort4>(arr_abstract, "<ArrayShort4>", "%d %d %d %d ");
    else

    if(arr_abstract->classType() == ArrayUByte1::Type())
      srfExport_Array1<ArrayUByte1>(arr_abstract, "<ArrayUByte1>", "%u ");
    else
    if(arr_abstract->classType() == ArrayUByte2::Type())
      srfExport_Array2<ArrayUByte2>(arr_abstract, "<ArrayUByte2>", "%u %u ");
    else
    if(arr_abstract->classType() == ArrayUByte3::Type())
      srfExport_Array3<ArrayUByte3>(arr_abstract, "<ArrayUByte3>", "%u %u %u ");
    else
    if(arr_abstract->classType() == ArrayUByte4::Type())
      srfExport_Array4<ArrayUByte4>(arr_abstract, "<ArrayUByte4>", "%u %u %u %u ");
    else

    if(arr_abstract->classType() == ArrayByte1::Type())
      srfExport_Array1<ArrayByte1>(arr_abstract, "<ArrayByte1>", "%d ");
    else
    if(arr_abstract->classType() == ArrayByte2::Type())
      srfExport_Array2<ArrayByte2>(arr_abstract, "<ArrayByte2>", "%d %d ");
    else
    if(arr_abstract->classType() == ArrayByte3::Type())
      srfExport_Array3<ArrayByte3>(arr_abstract, "<ArrayByte3>", "%d %d %d ");
    else
    if(arr_abstract->classType() == ArrayByte4::Type())
      srfExport_Array4<ArrayByte4>(arr_abstract, "<ArrayByte4>", "%d %d %d %d ");
    else

    if(arr_abstract->classType() == ArrayFloat1::Type())
      srfExport_Array1<ArrayFloat1>(arr_abstract, "<ArrayFloat1>", "%f ");
    else
    if(arr_abstract->classType() == ArrayFloat2::Type())
      srfExport_Array2<ArrayFloat2>(arr_abstract, "<ArrayFloat2>", "%f %f ");
    else
    if(arr_abstract->classType() == ArrayFloat3::Type())
      srfExport_Array3<ArrayFloat3>(arr_abstract, "<ArrayFloat3>", "%f %f %f ");
    else
    if(arr_abstract->classType() == ArrayFloat4::Type())
      srfExport_Array4<ArrayFloat4>(arr_abstract, "<ArrayFloat3>", "%f %f %f %f ");
    else

    if(arr_abstract->classType() == ArrayDouble1::Type())
      srfExport_Array1<ArrayDouble1>(arr_abstract, "<ArrayDouble1>", "%Lf ");
    else
    if(arr_abstract->classType() == ArrayDouble2::Type())
      srfExport_Array2<ArrayDouble2>(arr_abstract, "<ArrayDouble2>", "%Lf %Lf ");
    else
    if(arr_abstract->classType() == ArrayDouble3::Type())
      srfExport_Array3<ArrayDouble3>(arr_abstract, "<ArrayDouble3>", "%Lf %Lf %Lf ");
    else
    if(arr_abstract->classType() == ArrayDouble4::Type())
      srfExport_Array4<ArrayDouble4>(arr_abstract, "<ArrayDouble3>", "%Lf %Lf %Lf %Lf ");
    else
    {
      Log::error("Array type not supported for export.\n");
      VL_TRAP();
    }
  }

  void srfExport_DrawCallBase(DrawCall* dcall)
  {
      if (mObject_Ref_Count[dcall] > 1)
        mSRFString += indent() + "ID = " + getUID(dcall) + "\n";
      switch(dcall->primitiveType())
      {
        case PT_POINTS:                   mSRFString += indent() + "PrimitiveType = PT_POINTS\n"; break;
        case PT_LINES:                    mSRFString += indent() + "PrimitiveType = PT_LINES\n"; break;
        case PT_LINE_LOOP:                mSRFString += indent() + "PrimitiveType = PT_LINE_LOOP\n"; break;
        case PT_LINE_STRIP:               mSRFString += indent() + "PrimitiveType = PT_LINE_STRIP\n"; break;
        case PT_TRIANGLES:                mSRFString += indent() + "PrimitiveType = PT_TRIANGLES\n"; break;
        case PT_TRIANGLE_STRIP:           mSRFString += indent() + "PrimitiveType = PT_TRIANGLE_STRIP\n"; break;
        case PT_TRIANGLE_FAN:             mSRFString += indent() + "PrimitiveType = PT_TRIANGLE_FAN\n"; break;
        case PT_QUADS:                    mSRFString += indent() + "PrimitiveType = PT_QUADS\n"; break;
        case PT_QUAD_STRIP:               mSRFString += indent() + "PrimitiveType = PT_QUAD_STRIP\n"; break;
        case PT_POLYGON:                  mSRFString += indent() + "PrimitiveType = PT_POLYGON\n"; break;

        case PT_LINES_ADJACENCY:          mSRFString += indent() + "PrimitiveType = PT_LINES_ADJACENCY\n"; break;
        case PT_LINE_STRIP_ADJACENCY:     mSRFString += indent() + "PrimitiveType = PT_LINE_STRIP_ADJACENCY\n"; break;
        case PT_TRIANGLES_ADJACENCY:      mSRFString += indent() + "PrimitiveType = PT_TRIANGLES_ADJACENCY\n"; break;
        case PT_TRIANGLE_STRIP_ADJACENCY: mSRFString += indent() + "PrimitiveType = PT_TRIANGLE_STRIP_ADJACENCY\n"; break;

        case PT_PATCHES:                  mSRFString += indent() + "PrimitiveType = PT_PATCHES\n"; break;

        case PT_UNKNOWN:                  mSRFString += indent() + "PrimitiveType = PT_UNKNOWN\n"; break;
      }
      mSRFString += indent() + "Enabled = " + (dcall->isEnabled() ? "true" : "false") + "\n";

      if (dcall->patchParameter())
      {
        mSRFString += indent() + "PatchParameter = "; mAssign = true; srfExport_PatchParameter(dcall->patchParameter());
      }
  }

  void srfExport_PatchParameter(PatchParameter* pp)
  {
    if (isDefined(pp))
    {
      mSRFString += indent() + getUID(pp) + "\n";
      return;
    }
    else
      mAlreadyDefined.insert(pp);

    mSRFString += indent() + "<PatchParameter>\n";
    mSRFString += indent() + "{\n";
    ++mIndent;
    if (mObject_Ref_Count[pp] > 1)
      mSRFString += indent() + "ID = " + getUID(pp) + "\n";
    mSRFString += indent() + String::printf("PatchVertices = %d\n", pp->patchVertices());
    mSRFString += indent() + String::printf("PatchDefaultOuterLevel = ( %f %f %f %f )\n", pp->patchDefaultOuterLevel().x(), pp->patchDefaultOuterLevel().y(), pp->patchDefaultOuterLevel().z(), pp->patchDefaultOuterLevel().w() );
    mSRFString += indent() + String::printf("PatchDefaultInnerLevel = ( %f %f )\n",       pp->patchDefaultInnerLevel().x(), pp->patchDefaultInnerLevel().y() );
    --mIndent;
    mSRFString += indent() + "}\n";
  }

  void srfExport_DrawCall(DrawCall* dcall)
  {
    if (isDefined(dcall))
    {
      mSRFString += indent() + getUID(dcall) + "\n";
      return;
    }
    else
      mAlreadyDefined.insert(dcall);

    if (dcall->classType() == DrawArrays::Type())
    {
      DrawArrays* da = dcall->as<DrawArrays>();
      mSRFString += indent() + "<DrawArrays>\n";
      mSRFString += indent() + "{\n";
      ++mIndent;
      srfExport_DrawCallBase(da);
      mSRFString += indent() + String::printf("Instances = %d\n", da->instances());
      mSRFString += indent() + String::printf("Start = %d\n", da->start());
      mSRFString += indent() + String::printf("Count= %d\n", da->count());
      --mIndent;
      mSRFString += indent() + "}\n";
    }
    else
    if (dcall->classType() == DrawElementsUInt::Type())
    {
      DrawElementsUInt* de = dcall->as<DrawElementsUInt>();
      mSRFString += indent() + "<DrawElementsUInt>\n";
      mSRFString += indent() + "{\n";
      ++mIndent;
      srfExport_DrawCallBase(de);
      mSRFString += indent() + String::printf("Instances = %d\n", de->instances());
      mSRFString += indent() + "PrimitiveRestartEnabled = " + (de->primitiveRestartEnabled() ? "true\n" : "false\n");
      mSRFString += indent() + String::printf("BaseVertex = %d\n", de->baseVertex());
      mSRFString += indent() + "IndexBuffer = "; mAssign = true; srfExport_Array(de->indexBuffer());
      --mIndent;
      mSRFString += indent() + "}\n";
    }
    else
    if (dcall->classType() == DrawElementsUShort::Type())
    {
      DrawElementsUShort* de = dcall->as<DrawElementsUShort>();
      mSRFString += indent() + "<DrawElementsUShort>\n";
      mSRFString += indent() + "{\n";
      ++mIndent;
      srfExport_DrawCallBase(de);
      mSRFString += indent() + String::printf("Instances = %d\n", de->instances());
      mSRFString += indent() + "PrimitiveRestartEnabled = " + (de->primitiveRestartEnabled() ? "true\n" : "false\n");
      mSRFString += indent() + String::printf("BaseVertex = %d\n", de->baseVertex());
      mSRFString += indent() + "IndexBuffer = "; mAssign = true; srfExport_Array(de->indexBuffer());
      --mIndent;
      mSRFString += indent() + "}\n";
    }
    else
    if (dcall->classType() == DrawElementsUByte::Type())
    {
      DrawElementsUByte* de = dcall->as<DrawElementsUByte>();
      mSRFString += indent() + "<DrawElementsUByte>\n";
      mSRFString += indent() + "{\n";
      ++mIndent;
      srfExport_DrawCallBase(de);
      mSRFString += indent() + String::printf("Instances = %d\n", de->instances());
      mSRFString += indent() + "PrimitiveRestartEnabled = " + (de->primitiveRestartEnabled() ? "true\n" : "false\n");
      mSRFString += indent() + String::printf("BaseVertex = %d\n", de->baseVertex());
      mSRFString += indent() + "IndexBuffer = "; mAssign = true; srfExport_Array(de->indexBuffer());
      --mIndent;
      mSRFString += indent() + "}\n";
    }
    else
    if (dcall->classType() == MultiDrawElementsUInt::Type())
    {
      MultiDrawElementsUInt* de = dcall->as<MultiDrawElementsUInt>();
      mSRFString += indent() + "<MultiDrawElementsUInt>\n";
      mSRFString += indent() + "{\n";
      ++mIndent;
      srfExport_DrawCallBase(de);
      mSRFString += indent() + "PrimitiveRestartEnabled = " + (de->primitiveRestartEnabled() ? "true\n" : "false\n");
      mSRFString += indent() + "IndexBuffer = "; mAssign = true; srfExport_Array(de->indexBuffer());
      mSRFString += indent() + "CountVector = "; srfExport_vector1(de->countVector(), "%u ");
      mSRFString += indent() + "BaseVertices = "; srfExport_vector1(de->baseVertices(), "%u ");
      --mIndent;
      mSRFString += indent() + "}\n";
    }
    else
    if (dcall->classType() == MultiDrawElementsUShort::Type())
    {
      MultiDrawElementsUShort* de = dcall->as<MultiDrawElementsUShort>();
      mSRFString += indent() + "<MultiDrawElementsUShort>\n";
      mSRFString += indent() + "{\n";
      ++mIndent;
      srfExport_DrawCallBase(de);
      mSRFString += indent() + "PrimitiveRestartEnabled = " + (de->primitiveRestartEnabled() ? "true\n" : "false\n");
      mSRFString += indent() + "IndexBuffer = "; mAssign = true; srfExport_Array(de->indexBuffer());
      mSRFString += indent() + "CountVector = "; srfExport_vector1(de->countVector(), "%u ");
      mSRFString += indent() + "BaseVertices = "; srfExport_vector1(de->baseVertices(), "%u ");
      --mIndent;
      mSRFString += indent() + "}\n";
    }
    else
    if (dcall->classType() == MultiDrawElementsUByte::Type())
    {
      MultiDrawElementsUByte* de = dcall->as<MultiDrawElementsUByte>();
      mSRFString += indent() + "<MultiDrawElementsUByte>\n";
      mSRFString += indent() + "{\n";
      ++mIndent;
      srfExport_DrawCallBase(de);
      mSRFString += indent() + "PrimitiveRestartEnabled = " + (de->primitiveRestartEnabled() ? "true\n" : "false\n");
      mSRFString += indent() + "IndexBuffer = "; mAssign = true; srfExport_Array(de->indexBuffer());
      mSRFString += indent() + "CountVector = "; srfExport_vector1(de->countVector(), "%u ");
      mSRFString += indent() + "BaseVertices = "; srfExport_vector1(de->baseVertices(), "%u ");
      --mIndent;
      mSRFString += indent() + "}\n";
    }
    else
    {
      Log::error("DrawCall type not supported for export.\n");
      VL_TRAP();
    }
  }

  template<typename T>
  void srfExport_vector1(const std::vector<T>& vec, const char* format)
  {
    mSRFString += "( ";
    for(size_t i=0; i<vec.size(); ++i)
    {
      mSRFString += String::printf(format, vec[i]);
      /*if (i && (i%10==0) && i!=vec.size()-1)
        mSRFString += indent() + "\n"*/
    }
    mSRFString += " )\n";
  }

  String indent()
  {
    String str;
    if (mAssign)  
      mAssign = false;
    else
      for(int i=0; i<mIndent; ++i)
        str += '\t';
    return str;
  }

  String getUID(Object* object)
  {
    std::map< ref<Object>, String >::iterator it = mObject_To_UID.find(object);
    if( it != mObject_To_UID.end() )
      return it->second;
    else
    {
      VL_TRAP();
      return "";
    }
  }

  void generateUID(Object* object, const char* prefix)
  {
    if (object)
    {
      // add reference count
      ++mObject_Ref_Count[object];
      if (mObject_To_UID.find(object) == mObject_To_UID.end())
      {
        String uid = String::printf("#%sid%d", prefix, ++mUIDCounter);
        mObject_To_UID[object] = uid;
      }
    }
  }

  bool isDefined(Object* obj)
  {
    return mAlreadyDefined.find(obj) != mAlreadyDefined.end();
  }

  virtual void initEvent()
  {
    Log::notify(appletInfo());

    ref<Geometry> geom = makeIcosphere( vec3(0,0,0), 10, 0 );
    // ref<Geometry> geom = makeTeapot( vec3(0,0,0), 10, 4 );
    geom->computeNormals();
    geom->setColorArray( geom->normalArray() );
    // TriangleStripGenerator::stripfy(geom.get(), 22, false, false, true);
    // mic fixme: this does no realizes that we are using primitive restart
    // mic fixme: make this manage also MultiDrawElements
    // geom->makeGLESFriendly();

    ref<Effect> fx = new Effect;
    fx->shader()->enable(EN_LIGHTING);
    fx->shader()->enable(EN_DEPTH_TEST);
    fx->shader()->setRenderState( new Light, 0 );

    sceneManager()->tree()->addActor( geom.get(), fx.get(), NULL);

    // export init
    mUIDCounter = 0;
    mAssign = 0;
    mIndent = 0;

    //srfPrelink_Geometry(geom.get());
    //srfExport_Geometry(geom.get());
    //std::fstream fout;
    //fout.open("D:/VL/srf_export.vl", std::ios::out);
    //fout.write( mSRFString.toStdString().c_str(), mSRFString.length() );
    //fout.close();

    SRF_Parser parser;
    parser.tokenizer()->setInputFile( new DiskFile("D:/VL/srf_export.vl") );

    parser.parse();
    String dump = parser.dump();
    parser.link();

    // dump the dump
    {
      std::fstream fout;
      fout.open( "D:/VL/srf_export_dump.vl", std::ios::out );
      fout.write( dump.toStdString().c_str(), dump.length() );
      fout.close();
    }

    exit(0);
  }

protected:
  std::map< ref<Object>, String > mObject_To_UID;
  std::map< ref<Object>, int > mObject_Ref_Count;
  std::set< ref<Object> > mAlreadyDefined;
  int mUIDCounter;
  String mSRFString;
  int mIndent;
  bool mAssign;
};

// Have fun!

BaseDemo* Create_App_VLSRF() { return new App_VLSRF; }