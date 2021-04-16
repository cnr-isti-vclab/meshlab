#include "iomesh_plugin_container.h"

IOMeshPluginContainer::IOMeshPluginContainer()
{
}

size_t IOMeshPluginContainer::size() const
{
	return ioMeshPlugins.size();
}

void IOMeshPluginContainer::clear()
{
	ioMeshPlugins.clear();
	inputMeshFormatToPluginMap.clear();
	outputMeshFormatToPluginMap.clear();
	inputRasterFormatToPluginMap.clear();
}

void IOMeshPluginContainer::pushIOMeshPlugin(IOPlugin* iIOMesh)
{
	ioMeshPlugins.push_back(iIOMesh);

	//add input formats to inputFormatMap
	for (const FileFormat& ff : iIOMesh->importFormats()){
		for (const QString& currentExtension : ff.extensions) {
			if (! inputMeshFormatToPluginMap.contains(currentExtension.toLower())) {
				inputMeshFormatToPluginMap.insert(currentExtension.toLower(), iIOMesh);
			}
		}
	}

	//add output formats to outputFormatMap
	for (const FileFormat& ff : iIOMesh->exportFormats()){
		for (const QString& currentExtension : ff.extensions) {
			if (! outputMeshFormatToPluginMap.contains(currentExtension.toLower())) {
				outputMeshFormatToPluginMap.insert(currentExtension.toLower(), iIOMesh);
			}
		}
	}

	//add input raster formats to inputFormatMap
	for (const FileFormat& ff : iIOMesh->importRasterFormats()){
		for (const QString& currentExtension : ff.extensions) {
			if (! inputRasterFormatToPluginMap.contains(currentExtension.toLower())) {
				inputRasterFormatToPluginMap.insert(currentExtension.toLower(), iIOMesh);
			}
		}
	}
}

void IOMeshPluginContainer::eraseIOMeshPlugin(IOPlugin* iIOMesh)
{
	ioMeshPlugins.erase(std::find(ioMeshPlugins.begin(), ioMeshPlugins.end(), iIOMesh));
	for (const FileFormat& ff : iIOMesh->importFormats()){
		for (const QString& currentExtension : ff.extensions) {
			inputMeshFormatToPluginMap.remove(currentExtension.toLower());
		}
	}
	for (const FileFormat& ff : iIOMesh->exportFormats()){
		for (const QString& currentExtension : ff.extensions) {
			outputMeshFormatToPluginMap.remove(currentExtension.toLower());
		}
	}
	for (const FileFormat& ff : iIOMesh->importRasterFormats()){
		for (const QString& currentExtension : ff.extensions) {
			inputRasterFormatToPluginMap.remove(currentExtension.toLower());
		}
	}
}

bool IOMeshPluginContainer::isInputMeshFormatSupported(const QString& inputFormat) const
{
	return inputMeshFormatToPluginMap.find(inputFormat.toLower()) != inputMeshFormatToPluginMap.end();
}

bool IOMeshPluginContainer::isOutputMeshFormatSupported(const QString& outputFormat) const
{
	return outputMeshFormatToPluginMap.find(outputFormat.toLower()) != outputMeshFormatToPluginMap.end();
}

bool IOMeshPluginContainer::isInputRasterFormatSupported(const QString& inputFormat) const
{
	return inputRasterFormatToPluginMap.find(inputFormat.toLower()) != inputRasterFormatToPluginMap.end();
}

IOPlugin* IOMeshPluginContainer::inputMeshPlugin(const QString& inputFormat) const
{
	auto it = inputMeshFormatToPluginMap.find(inputFormat.toLower());
	if (it != inputMeshFormatToPluginMap.end())
		return *it;
	return nullptr;
}

IOPlugin* IOMeshPluginContainer::outputMeshPlugin(const QString& outputFormat) const
{
	auto it = outputMeshFormatToPluginMap.find(outputFormat.toLower());
	if (it != outputMeshFormatToPluginMap.end())
		return *it;
	return nullptr;
}

IOPlugin* IOMeshPluginContainer::inputRasterPlugin(const QString& inputFormat) const
{
	auto it = inputRasterFormatToPluginMap.find(inputFormat.toLower());
	if (it != inputRasterFormatToPluginMap.end())
		return *it;
	return nullptr;
}

QStringList IOMeshPluginContainer::inputMeshFormatList() const
{
	return inputMeshFormatToPluginMap.keys();
}

QStringList IOMeshPluginContainer::outputMeshFormatList() const
{
	return outputMeshFormatToPluginMap.keys();
}

QStringList IOMeshPluginContainer::inputRasterFormatList() const
{
	return inputRasterFormatToPluginMap.keys();
}

IOMeshPluginContainer::IOMeshPluginRangeIterator IOMeshPluginContainer::ioMeshPluginIterator(bool iterateAlsoDisabledPlugins) const
{
	return IOMeshPluginRangeIterator(this, iterateAlsoDisabledPlugins);
}

ConstPluginIterator<IOPlugin> IOMeshPluginContainer::IOMeshPluginRangeIterator::begin()
{
	return ConstPluginIterator<IOPlugin>(pm->ioMeshPlugins, pm->ioMeshPlugins.begin(), b);
}

ConstPluginIterator<IOPlugin> IOMeshPluginContainer::IOMeshPluginRangeIterator::end()
{
	return ConstPluginIterator<IOPlugin>(pm->ioMeshPlugins, pm->ioMeshPlugins.end(), b);
}

IOMeshPluginContainer::IOMeshPluginRangeIterator::IOMeshPluginRangeIterator(
		const IOMeshPluginContainer* pm,
		bool iterateAlsoDisabledPlugins) :
	pm(pm), b(iterateAlsoDisabledPlugins)
{
}
