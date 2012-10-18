/*
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2012, Bradley J Chambers
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "vtkPDALReader.h"

#include <map>
#include <string>

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkSmartPointer.h"
#include "vtkVertexGlyphFilter.h"

#include <boost/filesystem.hpp>

#include <pdal/Dimension.hpp>
#include <pdal/FileUtils.hpp>
#include <pdal/Options.hpp>
#include <pdal/PointBuffer.hpp>
#include <pdal/Reader.hpp>
#include <pdal/Schema.hpp>
#include <pdal/StageFactory.hpp>
#include <pdal/StageIterator.hpp>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPDALReader);

//----------------------------------------------------------------------------
vtkPDALReader::vtkPDALReader()
{
  this->FileName = 0;
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);
}

//----------------------------------------------------------------------------
vtkPDALReader::~vtkPDALReader()
{
  this->SetFileName(NULL);
}

//----------------------------------------------------------------------------
int vtkPDALReader::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (!this->GetFileName())
    {
    vtkErrorMacro("Filename is not set");
    return 0;
    }

  pdal::Options options;
    {
    options.add<std::string>("filename", this->GetFileName());
    }

  if (!pdal::FileUtils::fileExists(this->GetFileName()))
    {
    vtkErrorMacro("Could not find file " << this->GetFileName());
    return 0;
    }

    std::string ext = boost::filesystem::extension(this->GetFileName());

  if (ext == "")
    {
    vtkErrorMacro("Could not detect driver from extension ");
    return 0;
    }

  ext = ext.substr(1, ext.length() - 1);

  if (ext == "")
    {
    vtkErrorMacro("Could not detect driver from extension");
    return 0;
    }

  boost::to_lower(ext);

  std::map<std::string, std::string> drivers;
  drivers["las"] = "drivers.las.reader";
  drivers["laz"] = "drivers.las.reader";
  drivers["bin"] = "drivers.terrasolid.reader";
  drivers["qi"] = "drivers.qfit.reader";
  drivers["xml"] = "drivers.pipeline.reader";
  drivers["nitf"] = "drivers.nitf.reader";
  drivers["ntf"] = "drivers.nitf.reader";
//  drivers["bpf"] = "drivers.bpf.reader";

  std::string driver = drivers[ext];

  if (driver == "")
    {
    vtkErrorMacro("Could not determine file type of " << this->GetFileName());
    return 0;
    }

  pdal::StageFactory factory;
  pdal::Stage *reader = factory.createReader(driver, options);

  if (!reader)
    {
    vtkErrorMacro("Could not create reader");
    return 0;
    }

  try
    {
    reader->initialize();
    }
  catch (std::exception const &e)
    {
    vtkErrorMacro("Could not open PDAL file " << this->GetFileName());
    return 0;
    }

  const pdal::Schema &schema = reader->getSchema();

  vtkIdType nr_points = reader->getNumPoints();

  vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  points->SetDataTypeToFloat();
  points->SetNumberOfPoints(nr_points);

  pdal::PointBuffer data(schema, reader->getNumPoints());
  pdal::StageSequentialIterator *iter = reader->createSequentialIterator(data);
  boost::uint32_t numRead = iter->read(data);

  const pdal::Schema &buffer_schema = data.getSchema();
  const pdal::Dimension &dX = buffer_schema.getDimension("X");
  const pdal::Dimension &dY = buffer_schema.getDimension("Y");
  const pdal::Dimension &dZ = buffer_schema.getDimension("Z");
//  boost::optional<pdal::Dimension const &> dI = buffer_schema.getDimensionOptional("Intensity");

  std::cerr << dX.getMinimum() << " " << dX.getMaximum() << std::endl;
  std::cerr << dX.getNumericOffset() << " " << dX.getNumericScale() << std::endl;
  
  for (vtkIdType i = 0; i < nr_points; ++i)
    {
    boost::int32_t xi = data.getField<boost::int32_t>(dX, i);
    boost::int32_t yi = data.getField<boost::int32_t>(dY, i);
    boost::int32_t zi = data.getField<boost::int32_t>(dZ, i);

    double x = dX.applyScaling<boost::int32_t>(xi) - dX.getNumericOffset();
    double y = dY.applyScaling<boost::int32_t>(yi) - dY.getNumericOffset();
    double z = dZ.applyScaling<boost::int32_t>(zi) - dZ.getNumericOffset();

    double point[3] = { x, y, z };

//    double point[3] = { data.getField<double>(dX, i),
//                       data.getField<double>(dY, i),
//                       data.getField<double>(dZ, i)
//                     };
    points->SetPoint(i, point);
    if (i==0) {
      std::cerr << xi << "  " << x << std::endl;
      std::cerr << point[0] << " " << point[1] << " " << point[2] << std::endl;
    }
//    data.getField<float>(*dI, i);
    }

  polydata->SetPoints(points);

  vtkSmartPointer<vtkVertexGlyphFilter> vertexGlyphFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
#if VTK_MAJOR_VERSION <= 5
  vertexGlyphFilter->AddInput(polydata);
#else
  vertexGlyphFilter->AddInputData(polydata);
#endif
  vertexGlyphFilter->Update();

  output->ShallowCopy(vertexGlyphFilter->GetOutput());
  
  return 1;
}

//----------------------------------------------------------------------------
void vtkPDALReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
