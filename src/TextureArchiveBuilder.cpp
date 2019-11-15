#include <bsapacker/TextureArchiveBuilder.h>

#include <bsapacker/ArchiveBuilderHelper.h>
#include <QtConcurrent/QtConcurrentMap>
#include <QDirIterator>

#include "DirectXTex.h"

namespace BsaPacker
{
	TextureArchiveBuilder::TextureArchiveBuilder(const IArchiveBuilderHelper* archiveBuilderHelper, const QDir& rootDir)
		: m_ArchiveBuilderHelper(archiveBuilderHelper), m_RootDirectory(rootDir)
	{
		this->m_Cancelled = false;
		this->m_Archive = std::make_unique<BSArchiveAuto>(this->m_RootDirectory.path());
	}

	uint32_t TextureArchiveBuilder::setFiles()
	{
		uint32_t incompressibleFiles = 0;
		uint32_t compressibleFiles = 0;
		int count = 0;
		const QStringList& rootDirFilenames = this->m_ArchiveBuilderHelper->getRootDirectoryFilenames(this->m_RootDirectory);
		QDirIterator iterator(this->m_RootDirectory, QDirIterator::Subdirectories);
		while (iterator.hasNext()) {
			if (this->m_Cancelled) {
				return 0;
			}

			const QString& filepath = iterator.next();
			const bool ignored = this->m_ArchiveBuilderHelper->isFileIgnorable(filepath, rootDirFilenames);

			Q_EMIT valueChanged(++count);
			if (ignored) {
				continue;
			}

			this->m_ArchiveBuilderHelper->isIncompressible(filepath) ? ++incompressibleFiles : ++compressibleFiles;
			this->m_Archive->addFileFromDiskRoot(filepath);
		}
		this->m_Archive->setCompressed(true);
		QString path = this->m_RootDirectory.path();
		QString* pathPtr = &path;
		this->m_Archive->setDDSCallback(TextureArchiveBuilder::DDSCallback, static_cast<void*>(pathPtr));
		return incompressibleFiles + compressibleFiles;
	}

	void TextureArchiveBuilder::setShareData(const bool value)
	{
		this->m_Archive->setShareData(value);
	}

	std::unique_ptr<BSArchiveAuto> TextureArchiveBuilder::getArchive()
	{
		return std::move(this->m_Archive);
		//return std::unique_ptr<BSArchiveAuto>(this->m_Archive.get());
	}

	uint32_t TextureArchiveBuilder::getFileCount() const
	{
		return this->m_ArchiveBuilderHelper->getFileCount(this->m_RootDirectory);
	}

	void TextureArchiveBuilder::cancel()
	{
		this->m_Cancelled = true;
	}

	void TextureArchiveBuilder::DDSCallback(bsa_archive_t archive,
											const wchar_t* file_path,
											bsa_dds_info_t* dds_info,
											void* context)
	{
		auto image = std::make_unique<DirectX::ScratchImage>();
		DirectX::TexMetadata info;

		const QString path = *static_cast<QString*>(context) + '/' + QString::fromStdWString(file_path);
		const auto hr = LoadFromDDSFile(PREPARE_PATH_LIBBSARCH(path), DirectX::DDS_FLAGS_NONE, &info, *image);

		if (FAILED(hr))
			throw std::runtime_error("Failed to open DDS");

		dds_info->width = static_cast<uint32_t>(info.width);
		dds_info->height = static_cast<uint32_t>(info.height);
		dds_info->mipmaps = static_cast<uint32_t>(info.mipLevels);
	}
} // namespace BsaPacker