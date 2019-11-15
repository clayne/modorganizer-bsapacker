#include "WorkClass.h"

#include <QMessageBox>

#include <bsapacker/ArchiveBuildDirector.h>
#include <bsapacker/IModDto.h>
#include <bsapacker/ModDtoFactory.h>

#include <QtConcurrent>

namespace BsaPacker
{
	BsaPackerWorker::BsaPackerWorker(
			const ISettingsService* settingsService,
			const IModDtoFactory* modDtoFactory,
			const IArchiveBuilderFactory* archiveBuilderFactory,
			const IArchiveAutoService* archiveAutoService,
			const IDummyPluginServiceFactory* dummyPluginServiceFactory,
			const IHideLooseAssetService* hideLooseAssetService)
		: m_SettingsService(settingsService),
		  m_ModDtoFactory(modDtoFactory),
		  m_ArchiveBuilderFactory(archiveBuilderFactory),
		  m_ArchiveAutoService(archiveAutoService),
		  m_DummyPluginServiceFactory(dummyPluginServiceFactory),
		  m_HideLooseAssetService(hideLooseAssetService)
	{
	}

	void BsaPackerWorker::DoWork() const
	{
		const std::unique_ptr<IModDto> modDto = this->m_ModDtoFactory->Create(); // handles PackerDialog and validation, implements Null Object pattern
		const std::vector<bsa_archive_type_e> types = this->m_ArchiveBuilderFactory->GetArchiveTypes(modDto.get());
		for (auto&& type : types) {
			const std::unique_ptr<IArchiveBuilder> builder = this->m_ArchiveBuilderFactory->Create(type, modDto.get());
			ArchiveBuildDirector director(this->m_SettingsService, builder.get());
			director.Construct(); // must check if cancelled
			const std::unique_ptr<BSArchiveAuto> archive = builder->getArchive();
			if (archive) {
				const QString& archiveName = modDto->ArchiveName() + modDto->ArchiveExtension();
				this->m_ArchiveAutoService->CreateBSA(archive.get(), archiveName, type);
				QMessageBox::information(nullptr, "","Created " + archiveName);
			}
		}
		const std::unique_ptr<IDummyPluginService> pluginService = this->m_DummyPluginServiceFactory->Create();
		pluginService->CreatePlugin(modDto->Path(), modDto->ArchiveName());

		this->m_HideLooseAssetService->HideLooseAssets(modDto->Path());
	}
}

