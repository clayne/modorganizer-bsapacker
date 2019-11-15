#ifndef ARCHIVEBUILDERFACTORY_H
#define ARCHIVEBUILDERFACTORY_H


#include <bsapacker/IArchiveBuilderFactory.h>
#include <bsapacker/IArchiveBuilder.h>
#include <bsapacker/IArchiveBuilderHelper.h>
#include <bsapacker/IModDto.h>
#include <libbsarch/libbsarch.h>

namespace BsaPacker
{
	class ArchiveBuilderFactory : public IArchiveBuilderFactory
	{
	public:
		explicit ArchiveBuilderFactory(const IArchiveBuilderHelper* archiveBuilderHelper);
		~ArchiveBuilderFactory() override = default;
		// IArchiveBuilderFactory interface
		[[nodiscard]] std::vector<bsa_archive_type_e> GetArchiveTypes(const IModDto* modDto) const override;
		[[nodiscard]] std::unique_ptr<IArchiveBuilder> Create(bsa_archive_type_e archiveType, const IModDto* modDto) const override;

	private:
		const IArchiveBuilderHelper* m_ArchiveBuilderHelper = nullptr;
	};
} // namespace BsaPacker

#endif // ARCHIVEBUILDERFACTORY_H