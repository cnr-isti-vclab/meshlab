#include "io_plugin_container.h"

IOPluginContainer::IOPluginContainer()
{
}

size_t IOPluginContainer::size() const
{
	return ioPlugins.size();
}

void IOPluginContainer::clear()
{
	ioPlugins.clear();
	inputMeshFormatToPluginMap.clear();
	outputMeshFormatToPluginMap.clear();
	inputImageFormatToPluginMap.clear();
}

void IOPluginContainer::pushIOPlugin(IOPlugin* iIO)
{
	ioPlugins.push_back(iIO);

	//add input formats to inputFormatMap
	for (const FileFormat& ff : iIO->importFormats()){
		for (const QString& currentExtension : ff.extensions) {
			if (! inputMeshFormatToPluginMap.contains(currentExtension.toLower())) {
				inputMeshFormatToPluginMap.insert(currentExtension.toLower(), iIO);
			}
		}
	}

	//add output formats to outputFormatMap
	for (const FileFormat& ff : iIO->exportFormats()){
		for (const QString& currentExtension : ff.extensions) {
			if (! outputMeshFormatToPluginMap.contains(currentExtension.toLower())) {
				outputMeshFormatToPluginMap.insert(currentExtension.toLower(), iIO);
			}
		}
	}

	//add input image formats to inputFormatMap
	for (const FileFormat& ff : iIO->importImageFormats()){
		for (const QString& currentExtension : ff.extensions) {
			if (! inputImageFormatToPluginMap.contains(currentExtension.toLower())) {
				inputImageFormatToPluginMap.insert(currentExtension.toLower(), iIO);
			}
		}
	}

	//add output image formats to inputFormatMap
	for (const FileFormat& ff : iIO->exportImageFormats()){
		for (const QString& currentExtension : ff.extensions) {
			if (! outputImageFormatToPluginMap.contains(currentExtension.toLower())) {
				outputImageFormatToPluginMap.insert(currentExtension.toLower(), iIO);
			}
		}
	}

	//add input project formats to inputFormatMap
	for (const FileFormat& ff : iIO->importProjectFormats()){
		for (const QString& currentExtension : ff.extensions) {
			if (! inputProjectFormatToPluginMap.contains(currentExtension.toLower())) {
				inputProjectFormatToPluginMap.insert(currentExtension.toLower(), iIO);
			}
		}
	}

	//add output project formats to inputFormatMap
	for (const FileFormat& ff : iIO->exportProjectFormats()){
		for (const QString& currentExtension : ff.extensions) {
			if (! outputProjectFormatToPluginMap.contains(currentExtension.toLower())) {
				outputProjectFormatToPluginMap.insert(currentExtension.toLower(), iIO);
			}
		}
	}
}

void IOPluginContainer::eraseIOPlugin(IOPlugin* iIO)
{
	ioPlugins.erase(std::find(ioPlugins.begin(), ioPlugins.end(), iIO));
	for (const FileFormat& ff : iIO->importFormats()){
		for (const QString& currentExtension : ff.extensions) {
			inputMeshFormatToPluginMap.remove(currentExtension.toLower());
		}
	}
	for (const FileFormat& ff : iIO->exportFormats()){
		for (const QString& currentExtension : ff.extensions) {
			outputMeshFormatToPluginMap.remove(currentExtension.toLower());
		}
	}
	for (const FileFormat& ff : iIO->importImageFormats()){
		for (const QString& currentExtension : ff.extensions) {
			inputImageFormatToPluginMap.remove(currentExtension.toLower());
		}
	}
}

bool IOPluginContainer::isInputMeshFormatSupported(const QString& inputFormat) const
{
	return inputMeshFormatToPluginMap.find(inputFormat.toLower()) != inputMeshFormatToPluginMap.end();
}

bool IOPluginContainer::isOutputMeshFormatSupported(const QString& outputFormat) const
{
	return outputMeshFormatToPluginMap.find(outputFormat.toLower()) != outputMeshFormatToPluginMap.end();
}

bool IOPluginContainer::isInputImageFormatSupported(const QString& inputFormat) const
{
	return inputImageFormatToPluginMap.find(inputFormat.toLower()) != inputImageFormatToPluginMap.end();
}

bool IOPluginContainer::isOutputImageFormatSupported(const QString& outputFormat) const
{
	return outputImageFormatToPluginMap.find(outputFormat.toLower()) != outputImageFormatToPluginMap.end();
}

bool IOPluginContainer::isInputProjectFormatSupported(const QString& inputFormat) const
{
	return inputProjectFormatToPluginMap.find(inputFormat.toLower()) != inputProjectFormatToPluginMap.end();
}

bool IOPluginContainer::isOutputProjectFormatSupported(const QString& outputFormat) const
{
	return outputImageFormatToPluginMap.find(outputFormat.toLower()) != outputProjectFormatToPluginMap.end();
}

IOPlugin* IOPluginContainer::inputMeshPlugin(const QString& inputFormat) const
{
	auto it = inputMeshFormatToPluginMap.find(inputFormat.toLower());
	if (it != inputMeshFormatToPluginMap.end())
		return *it;
	return nullptr;
}

IOPlugin* IOPluginContainer::outputMeshPlugin(const QString& outputFormat) const
{
	auto it = outputMeshFormatToPluginMap.find(outputFormat.toLower());
	if (it != outputMeshFormatToPluginMap.end())
		return *it;
	return nullptr;
}

IOPlugin* IOPluginContainer::inputImagePlugin(const QString& inputFormat) const
{
	auto it = inputImageFormatToPluginMap.find(inputFormat.toLower());
	if (it != inputImageFormatToPluginMap.end())
		return *it;
	return nullptr;
}

IOPlugin* IOPluginContainer::outputImagePlugin(const QString& outputFormat) const
{
	auto it = outputImageFormatToPluginMap.find(outputFormat.toLower());
	if (it != outputImageFormatToPluginMap.end())
		return *it;
	return nullptr;
}

IOPlugin* IOPluginContainer::inputProjectPlugin(const QString& inputFormat) const
{
	auto it = inputProjectFormatToPluginMap.find(inputFormat.toLower());
	if (it != inputProjectFormatToPluginMap.end())
		return *it;
	return nullptr;
}

IOPlugin* IOPluginContainer::outputProjectPlugin(const QString& outputFormat) const
{
	auto it = outputProjectFormatToPluginMap.find(outputFormat.toLower());
	if (it != outputProjectFormatToPluginMap.end())
		return *it;
	return nullptr;
}

QStringList IOPluginContainer::inputMeshFormatList() const
{
	return inputMeshFormatToPluginMap.keys();
}

QStringList IOPluginContainer::outputMeshFormatList() const
{
	return outputMeshFormatToPluginMap.keys();
}

QStringList IOPluginContainer::inputImageFormatList() const
{
	return inputImageFormatToPluginMap.keys();
}

QStringList IOPluginContainer::outputImageFormatList() const
{
	return outputImageFormatToPluginMap.keys();
}

QStringList IOPluginContainer::inputProjectFormatList() const
{
	return inputProjectFormatToPluginMap.keys();
}

QStringList IOPluginContainer::outputProjectFormatList() const
{
	return outputProjectFormatToPluginMap.keys();
}

IOPluginContainer::IOPluginRangeIterator IOPluginContainer::ioPluginIterator(bool iterateAlsoDisabledPlugins) const
{
	return IOPluginRangeIterator(this, iterateAlsoDisabledPlugins);
}

ConstPluginIterator<IOPlugin> IOPluginContainer::IOPluginRangeIterator::begin()
{
	return ConstPluginIterator<IOPlugin>(pm->ioPlugins, pm->ioPlugins.begin(), b);
}

ConstPluginIterator<IOPlugin> IOPluginContainer::IOPluginRangeIterator::end()
{
	return ConstPluginIterator<IOPlugin>(pm->ioPlugins, pm->ioPlugins.end(), b);
}

IOPluginContainer::IOPluginRangeIterator::IOPluginRangeIterator(
		const IOPluginContainer* pm,
		bool iterateAlsoDisabledPlugins) :
	pm(pm), b(iterateAlsoDisabledPlugins)
{
}
